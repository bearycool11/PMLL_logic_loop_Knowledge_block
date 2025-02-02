{
  "devContainerConfig": {
    "description": "Configuration for development container environment",
    "type": "object",
    "properties": {
      "image": {
        "description": "Docker image to use for the development container",
        "type": "string"
      },
      "features": {
        "description": "Additional features to include in the container",
        "type": "array",
        "items": {
          "type": "string"
        }
      },
      "settings": {
        "description": "VS Code settings specific to the container",
        "type": "object",
        "additionalProperties": {
          "type": "string"
        }
      },
      "extensions": {
        "description": "List of VS Code extensions to install in the container",
        "type": "array",
        "items": {
          "type": "string"
        }
      },
      "mounts": {
        "description": "File system mounts to be included in the container",
        "type": "array",
        "items": {
          "type": "string"
        }
      }
    },
    "required": ["image"]
  }
}
