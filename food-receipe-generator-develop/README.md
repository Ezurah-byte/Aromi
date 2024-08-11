# Nuvolux - A Food Recipe Generator

**Nuvolux** is a personalized cooking companion, powered by the Gemini AI API, that helps you discover new recipes, explore flavors, and cook with confidence. By suggesting recipes based on the ingredients you have, it unleashes your culinary creativity. 

## Features

- **Ingredient-Based Recipe Suggestions:** Suggests recipes using only the ingredients you provide, ensuring no additional items are needed.
- **Detailed Recipe Information:** Each recipe comes with step-by-step instructions, nutritional information, and a brief summary of health benefits.
- **Image Search Integration:** Automatically retrieves a relevant image for each dish from Google Image Search.
- **User-Friendly Interface:** Simple and intuitive design that focuses on user experience.

## Table of Contents

- [Introduction](#introduction)
- [Key Functionalities](#Key-Functionalities)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [API Endpoints](#api-endpoints)
- [Docker Setup](#docker-setup)
- [Environment Variables](#environment-variables)


## Introduction

The Recipe Suggestion API is a Flask-based application that utilizes Google Custom Search and Google Generative AI to provide unique recipe ideas. It supports storing recipes in MongoDB and offers several endpoints for interacting with the stored data.

## Key Functionalities

- **Google API Integration**: Fetch recipes and images using Google APIs.
- **MongoDB Storage**: Store and retrieve recipes with MongoDB.
- **Health Check Endpoint**: Quickly verify the API's operational status.
- **Dockerized Deployment**: Simplify environment setup and scaling with Docker.

## Requirements

Before installing, ensure you have the following dependencies:

- **Python**: Version 3.8 or above
- **Docker**: For containerized deployment
- **MongoDB**: Database for storing recipes
- **Google Cloud Account**: Access to Google APIs

## Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/yourusername/recipe-suggestion-api.git
cd recipe-suggestion-api
```

### Step 2: Create a Virtual Environment

```bash
python -m venv env
source env/bin/activate  # On Windows use `env\Scripts\activate`
```

### Step 3: Install Dependencies

```bash
pip install -r requirements.txt
```

### Step 4: Set Up Environment Variables

Create a .env file in the root directory based on the env_sample file provided. Fill in your API keys and database information.

```bash
GEMINI_API_KEY=<your-api-key>
MONGO_URI=<mongo-connection-url>
MONGO_DB=<db-name>
SEARCH_ENGINE_ID=<programmable-search-engine-id>
GOOGLE_API_KEY=<google-cloud-console-api>
```

### Step 5: Run the Application

```bash
python main.py
```

## Usage

Once the application is running, you can access the API at `http://127.0.0.1:5000`.

### Example Endpoints

- **Health Check**:
  ```bash
  curl http://127.0.0.1:5000/health_check
  ```

  Response :
  ```
  success
  ```
  
- **Get Suggested Recipes**:
  ```bash
  curl -X POST "http://127.0.0.1:5000/get_suggested_recipes
  ```

  Request :
  ```
  --header 'Content-Type: application/json' \
  --data '{
           "input":["chilli","cheese","salt","pasta","pepper"]
          }'
  ```
  Response :
  ```
  {
    "recipe_id": "66b4908a6c69cb9f8d2165cb",
    "recipes": {
        "recipes1": {
            "name": "Spicy Cheese Pasta",
            "category": [
                "Main Course",
                "Italian",
                "Pasta Dishes"
            ],
            "Nutrition_Per_Serving": {
                "calories": "400",
                "fat": "15g",
                "cholesterol": "30mg",
                "sodium": "300mg",
                "protein": "20g"
            },
            "instructions": [
                "Cook pasta according to package directions.",
                "While pasta is cooking, grate cheese.",
                "In a large skillet, heat olive oil over medium heat.",
                "Add chilli flakes and cook for 1 minute, stirring frequently.",
                "Add pasta, cheese, salt, and pepper.",
                "Toss until cheese is melted and pasta is coated."
            ],
            "health_benefits": [
                "Provides a good source of protein and carbohydrates.",
                "Chilli flakes can help improve digestion and circulation."
            ],
            "Ratings": "4",
            "Short_Description": "A simple, spicy pasta dish with a cheesy kick.",
            "Duration": "20 minutes",
            "image_url": "https://i.ytimg.com/vi/X-Z6mtWvXM4/maxresdefault.jpg"
         }
      }
  }
  ```

- **Get Recipes from Database**:
  ```bash
  curl -X POST "http://127.0.0.1:5000/recipes"
  ```

  Response :
  ```
  {
    "recipe_ids": [
        "669108f44d12fa4a25994d74",
        "6691097e4d12fa4a25994d75",
        "66acb07805fa0bf31eff80a5",
        "66acb2294962c51ddea66d12",
        "66acb3cd0c731b360c0f6522",
        "66acbf3f8e78e8d15adc2fdf",
        "66acbf778e78e8d15adc2fe0",
        "66acc26406e4e2aacf8cb1e0",
    ]
  }
  ```

## API Endpoints

### /health_check
- **Method**:GET
- **Description**:Checks the API's operational status
- **Response**: 200 OK: "API is up and running."

### /get_suggested_recipes
- **Method**:POST
- **Parameters**: Ingredients (required): Comma-separated list of ingredients.
- **Description**: Fetches recipe suggestions based on the provided ingredients using Google Gemini AI Model.
- **Response**: 200 OK: JSON array of suggested recipes."
- **Response**: 400 Bad Request: "Missing ingredients parameter."
- **Response**: 500 Internal Server Error: "Error processing the request."

### /get_recipes
- **Method**:POST
- **Parameters**: Recipe (required): recipe name
- **Description**: Fetches recipe based on the provided recipe name using Google Gemini AI Model.
- **Response**: 200 OK: JSON array of suggested recipes."
- **Response**: 400 Bad Request: "Missing ingredients parameter."
- **Response**: 500 Internal Server Error: "Error processing the request."

### /recipes
- **Method**:GET
- **Description**:Retrieves all stored recipes from the MongoDB database.
- **Response**: 200 OK: "JSON array of stored recipes."
- **Response**: 500 Internal Server Error: "Error retrieving recipes."

### /recipes/<recipe_id>
- **Method**:GET
- **Description**:Retrieves stored id specified recipe from the MongoDB database.
- **Response**: 200 OK: "JSON array of stored recipes."
- **Response**: 500 Internal Server Error: "Error retrieving recipes id."

## Docker Setup

To run the application using Docker, ensure Docker is installed

- **Build the Docker Image**: Create a Docker image locally.
- **Run the Docker Container**: Launch application within Docker container.

## Environment Variables

Variable	Description:
- **GEMINI_API_KEY**:	API key for Google Generative AI.
- **MONGO_URI**:	MongoDB connection URI.
- **MONGO_DB**:	Name of the MongoDB database.
- **SEARCH_ENGINE_ID**:	Google Programmable Search Engine ID.
- **GOOGLE_API_KEY**:	Google Cloud Console API key.