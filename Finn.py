from fastapi import FastAPI, Depends, HTTPException
from pydantic import BaseModel
import pickle
from typing import List
import json

# Initialize FastAPI
app = FastAPI()

# ---- Core Brain Components (PMLL, ARLL, EFLL) ----
class PMLLMemory:
    """Persistent Memory to store historical data."""
    def __init__(self):
        self.history = []

    def save(self, input_text, prediction, feedback=None):
        entry = {"text": input_text, "prediction": prediction, "feedback": feedback}
        self.history.append(entry)

    def get_history(self):
        return self.history


class ARLL:
    """Adaptive Reinforcement Learning for model improvement."""
    def __init__(self, model):
        self.model = model
        self.feedback_data = []

    def record_feedback(self, input_text, true_sentiment):
        self.feedback_data.append((input_text, true_sentiment))

    def retrain_model(self):
        # Simulated model improvement logic
        print(f"Retraining model with {len(self.feedback_data)} feedback entries.")


class EFLL:
    """Emotional Feedback Learning for empathetic responses."""
    def provide_emotional_feedback(self, sentiment):
        feedback_map = {
            "positive": "That's great to hear!",
            "negative": "I'm sorry to hear that. Let me know how I can help.",
            "neutral": "Thanks for sharing your thoughts."
        }
        return feedback_map.get(sentiment, "Thank you for your input.")

# ---- Initialize Framework Components ----
pmll_memory = PMLLMemory()
arll = ARLL(model=None)  # Model will be loaded dynamically
efll = EFLL()

# ---- Model Loading ----
def load_model():
    try:
        with open("sentiment_model.pkl", "rb") as f:
            return pickle.load(f)
    except FileNotFoundError:
        raise HTTPException(status_code=500, detail="Model file not found.")

@app.on_event("startup")
def startup_event():
    global model
    model = load_model()
    arll.model = model  # Link the loaded model to ARLL


# ---- FastAPI Endpoints ----
class TextInput(BaseModel):
    text: str

class FeedbackInput(BaseModel):
    text: str
    true_sentiment: str

@app.post("/predict")
async def predict(input: TextInput, model=Depends(load_model)):
    try:
        prediction = model.predict([input.text])[0]
        pmll_memory.save(input.text, prediction)  # Store in PMLL
        emotional_feedback = efll.provide_emotional_feedback(prediction)  # Generate EFLL response
        return {"text": input.text, "sentiment": prediction, "emotional_feedback": emotional_feedback}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.post("/feedback")
async def feedback(input: FeedbackInput):
    arll.record_feedback(input.text, input.true_sentiment)
    pmll_memory.save(input.text, None, feedback=input.true_sentiment)  # Log feedback in PMLL
    return {"message": "Feedback recorded. Thank you!"}

@app.post("/retrain")
async def retrain_model():
    arll.retrain_model()
    return {"message": "Model retrained successfully."}

@app.get("/history")
async def get_history():
    return pmll_memory.get_history()

# ---- Finn Initialization ----
@app.get("/")
async def root():
    return {"message": "Welcome to Finn, the Brain of ChatGPT and OpenAI."}
