git     pml_logic_loop->memory_silo_id = 1;
    pml_logic_loop->io_socket_id = io_socket_id;
    pml_logic_loop->free_c_present = 1; // Set the flag for demonstration
  }
  return pml_logic_loop;
}

// Function to cleanup the memory and socket
void pml_logic_loop_cleanup() {
  if (pml_logic_loop != NULL) {
    close(pml_logic_loop->io_socket_id); // Close the socket
    free(pml_logic_loop); // Free the memory allocated for the PML logic loop
    pml_logic_loop = NULL;
  }
}

// Main program (example usage)
int main() {
  int socket_id = init_socket("127.0.0.1", 8080); // Initialize socket with example IP and port
  if (socket_id < 0) {
    fprintf(stderr, "Socket initialization failed.\n");
    return EXIT_FAILURE;
  }

  // Initialize the PML logic loop with sample memory silo ID and socket ID
  pml_logic_loop_init(1, socket_id);
  
  // Example buffer (can be replaced with actual data)
  char buffer[1024] = "Example data";
  pml_logic_loop_process(socket_id, buffer, sizeof(buffer));
  
  // Cleanup before exit
  pml_logic_loop_cleanup();

  return EXIT_SUCCESS;
}
