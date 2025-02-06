using System;
using System.IO;
using System.Text;
using System.Text.Json;
using System.Net.WebSockets;
using System.Threading;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Collections.Concurrent;
using Microsoft.AspNetCore.Http;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.DurableTask;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.Extensions.Logging;
using StackExchange.Redis;
using Nethereum.Web3;
using Nethereum.Web3.Accounts;
using OpenAI_API;
using Azure;
using Azure.AI.TextAnalytics;
using Azure.AI.Speech;
using Azure.AI.Speech.Audio;
using RestSharp;
using Neo4j.Driver;

namespace CopilotAzureChatGPT5o
{
    public static class CopilotAzureAI
    {
        // ====== Redis Connection (Multi-User AI Sessions) ======
        private static readonly string RedisHost = Environment.GetEnvironmentVariable("REDIS_HOST");
        private static readonly int RedisPort = int.Parse(Environment.GetEnvironmentVariable("REDIS_PORT"));
        private static readonly ConnectionMultiplexer Redis = ConnectionMultiplexer.Connect($"{RedisHost}:{RedisPort}");

        [FunctionName("StoreUserSession")]
        public static async Task StoreUserSession([ActivityTrigger] (string userId, string message) data, ILogger log)
        {
            try
            {
                var db = Redis.GetDatabase();
                string sessionKey = $"session:{data.userId}";

                List<string> messages = await GetUserSession(data.userId) ?? new List<string>();
                messages.Add(data.message);

                await db.StringSetAsync(sessionKey, JsonSerializer.Serialize(messages));
                log.LogInformation($"[Session] Stored for {data.userId}: {data.message}");
            }
            catch (RedisException ex)
            {
                log.LogError($"Redis error (StoreUserSession): {ex.Message}");
                throw;
            }
            catch (Exception ex)
            {
                log.LogError($"Unexpected error (StoreUserSession): {ex.Message}");
                throw;
            }
        }

        [FunctionName("RetrieveUserSession")]
        public static async Task<List<string>> RetrieveUserSession([ActivityTrigger] string userId, ILogger log)
        {
            try
            {
                var sessionData = await GetUserSession(userId);
                return sessionData ?? new List<string>();
            }
            catch (RedisException ex)
            {
                log.LogError($"Redis error (RetrieveUserSession): {ex.Message}");
                throw;
            }
            catch (Exception ex)
            {
                log.LogError($"Unexpected error (RetrieveUserSession): {ex.Message}");
                throw;
            }
        }

        private static async Task<List<string>> GetUserSession(string userId)
        {
            var db = Redis.GetDatabase();
            string sessionKey = $"session:{userId}";
            string sessionData = await db.StringGetAsync(sessionKey);
            return sessionData != null
                ? JsonSerializer.Deserialize<List<string>>(sessionData)
                : null;
        }

        // ====== Blockchain AI Memory (Ethereum) ======
        private static readonly string NodeUrl = Environment.GetEnvironmentVariable("ETH_NODE_URL");
        private static readonly string PrivateKey = Environment.GetEnvironmentVariable("ETH_PRIVATE_KEY");
        private static readonly Web3 Web3 = new(new Account(PrivateKey), NodeUrl);

        // Update with your actual deployed contract address
        private static readonly string ContractAddress = "YOUR_DEPLOYED_CONTRACT_ADDRESS";

        [FunctionName("StoreMemoryBlockchain")]
        public static async Task StoreMemoryBlockchain([ActivityTrigger] (string input, string response) memory, ILogger log)
        {
            try
            {
                var contract = Web3.Eth.GetContractHandler(ContractAddress);
                var transaction = await contract.SendTransactionAsync("storeMemory", memory.input, memory.response);
                log.LogInformation($"[Blockchain] Memory stored on Ethereum: {transaction}");
            }
            catch (Exception ex)
            {
                log.LogError($"[Blockchain] Error storing memory: {ex.Message}");
                throw;
            }
        }

        // ====== AI-Powered Image Generation (DALL·E) ======
        [FunctionName("GenerateAIImage")]
        public static async Task<string> GenerateAIImage([ActivityTrigger] string prompt, ILogger log)
        {
            try
            {
                string apiKey = Environment.GetEnvironmentVariable("OPENAI_API_KEY");
                var client = new OpenAIAPI(apiKey);
                var result = await client.Image.CreateImageAsync(prompt, 1, "1024x1024");

                if (result?.Data?.Count > 0)
                {
                    string imageUrl = result.Data[0].Url;
                    log.LogInformation($"[DALL·E] Generated Image: {imageUrl}");
                    return imageUrl;
                }
                else
                {
                    log.LogWarning("[DALL·E] No image data received from OpenAI API.");
                    return null;
                }
            }
            catch (Exception ex)
            {
                log.LogError($"OpenAI API error (GenerateAIImage): {ex.Message}");
                throw;
            }
        }

        // ====== AI-Powered Video Generation (RunwayML) ======
        [FunctionName("GenerateAIVideo")]
        public static async Task<string> GenerateAIVideo([ActivityTrigger] string description, ILogger log)
        {
            try
            {
                var client = new RestClient("https://api.runwayml.com/v1/videos");
                var request = new RestRequest(Method.POST);
                request.AddHeader("Authorization", $"Bearer {Environment.GetEnvironmentVariable("RUNWAYML_API_KEY")}");
                request.AddJsonBody(new { prompt = description });

                var response = await client.ExecuteAsync(request);
                if (response.IsSuccessful)
                {
                    log.LogInformation($"[RunwayML] Video Response: {response.Content}");
                    return response.Content;
                }
                else
                {
                    log.LogError($"[RunwayML] API error: {response.ErrorMessage}");
                    return null;
                }
            }
            catch (Exception ex)
            {
                log.LogError($"[RunwayML] Exception in GenerateAIVideo: {ex.Message}");
                throw;
            }
        }

        // ====== Emotion Recognition AI (Azure Text Analytics) ======
        private static readonly string _textAnalyticsKey = Environment.GetEnvironmentVariable("TEXT_ANALYTICS_KEY");
        private static readonly string _textAnalyticsEndpoint = Environment.GetEnvironmentVariable("TEXT_ANALYTICS_ENDPOINT");
        private static readonly TextAnalyticsClient _textClient =
            new(new Uri(_textAnalyticsEndpoint), new AzureKeyCredential(_textAnalyticsKey));

        [FunctionName("AnalyzeSentiment")]
        public static async Task<string> AnalyzeSentiment([ActivityTrigger] string message, ILogger log)
        {
            try
            {
                DocumentSentiment sentiment = await _textClient.AnalyzeSentimentAsync(message);
                string emotion = sentiment.Sentiment.ToString();
                log.LogInformation($"[Sentiment] Message: '{message}' → Emotion: {emotion}");
                return emotion;
            }
            catch (Exception ex)
            {
                log.LogError($"[Sentiment] Error analyzing sentiment: {ex.Message}");
                throw;
            }
        }

        // ====== Voice AI (Speech-To-Text & Text-To-Speech) ======
        private static readonly SpeechConfig _speechConfig = SpeechConfig.FromSubscription(
            Environment.GetEnvironmentVariable("SPEECH_KEY"),
            Environment.GetEnvironmentVariable("SPEECH_REGION"));

        [FunctionName("SpeechToText")]
        public static async Task<string> SpeechToText([ActivityTrigger] byte[] audioData, ILogger log)
        {
            // Example approach with an in-memory stream:
            // In a real scenario, you might need a Push/PullAudioInputStream for the audio data.
            using var audioStream = new MemoryStream(audioData);
            using var audioConfig = AudioConfig.FromStreamInput(audioStream);
            using var recognizer = new SpeechRecognizer(_speechConfig, audioConfig);

            var result = await recognizer.RecognizeOnceAsync();
            log.LogInformation($"[STT] Recognized: {result.Text}");
            return result.Text;
        }

        [FunctionName("TextToSpeech")]
        public static async Task<byte[]> TextToSpeech([ActivityTrigger] string text, ILogger log)
        {
            using var synthesizer = new SpeechSynthesizer(_speechConfig);
            var result = await synthesizer.SpeakTextAsync(text);
            log.LogInformation($"[TTS] Synthesized speech for: {text}");
            return result.AudioData;
        }

        // ====== WebSocket Handler (Thread-Safe) ======
        private static readonly ConcurrentDictionary<string, WebSocket> _activeSockets = new();

        [FunctionName("WebSocketHandler")]
        public static async Task WebSocketFunction(
            [HttpTrigger(AuthorizationLevel.Function, "get")] HttpRequest req,
            ILogger log)
        {
            if (!req.HttpContext.WebSockets.IsWebSocketRequest) return;

            WebSocket socket = await req.HttpContext.WebSockets.AcceptWebSocketAsync();
            string connectionId = Guid.NewGuid().ToString();

            _activeSockets[connectionId] = socket;
            log.LogInformation($"[WebSocket] Connection established: {connectionId}");

            await HandleWebSocketConnection(socket, connectionId, log);
        }

        private static async Task HandleWebSocketConnection(WebSocket socket, string connectionId, ILogger log)
        {
            var buffer = new byte[4096];

            while (socket.State == WebSocketState.Open)
            {
                WebSocketReceiveResult result;
                try
                {
                    result = await socket.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
                }
                catch (Exception ex)
                {
                    log.LogError($"[WebSocket] Receive error: {ex.Message}");
                    break;
                }

                if (result.MessageType == WebSocketMessageType.Close)
                {
                    _activeSockets.TryRemove(connectionId, out _);
                    await socket.CloseAsync(WebSocketCloseStatus.NormalClosure, "Closing", CancellationToken.None);
                    log.LogInformation($"[WebSocket] Connection closed: {connectionId}");
                    break;
                }

                // Process incoming message
                string receivedMessage = Encoding.UTF8.GetString(buffer, 0, result.Count);
                // Mock AI response
                string aiResponse = $"AI Response: {receivedMessage} (processed in real-time)";

                var responseBuffer = Encoding.UTF8.GetBytes(aiResponse);
                await socket.SendAsync(new ArraySegment<byte>(responseBuffer), WebSocketMessageType.Text, true, CancellationToken.None);
            }
        }
    }
}
