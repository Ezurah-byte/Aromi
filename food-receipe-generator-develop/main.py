from flask import Flask, request, jsonify
import google.generativeai as genai
import json
from pymongo import MongoClient
from bson import json_util
from bson.objectid import ObjectId
from dotenv import load_dotenv
import os
import logging
from googleapiclient.discovery import build

google = os.getenv("GOOGLE_API_KEY")
search_engine = os.getenv("SEARCH_ENGINE_ID")




logging.basicConfig(filename='log.txt', level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
load_dotenv(override=True)
app = Flask(__name__)


client = MongoClient(os.getenv("MONGO_URI"))
db = client[os.getenv("MONGO_DB")]
collection = db["recipe_history"]

@app.route('/health_check', methods=['GET'])
def health_check():
    logging.info("Healthcheck pinged")
    return "success"

def get_image_url(search_term):
    service = build("customsearch", "v1", developerKey=google)
    # Making an API call to get image results
    result = service.cse().list(
        q=search_term,
        cx=search_engine,
        searchType="image",
        num=1
    ).execute()
    
    # Checking if 'items' exist in the result and returning the first image link
    if "items" in result:
        return result["items"][0]["link"]
    return None

@app.route('/get_suggested_recipes', methods=['POST'])
def get_suggested_recipes():
    try:
        data = request.get_json()
        ingredients = data.get('input')
        logging.info("Received data: %s", data)

        if not ingredients or not isinstance(ingredients, list):
            logging.error("Invalid input: Ingredients are required and must be a list.")
            return jsonify({"output": "Input is required and must be a list of ingredients."}), 400

        # Generate the prompt with the given ingredients
        prompt_template = """
        You are a skilled chef AI in creating recipes with the provided ingredients.If the input is a list of specific ingredient name , suggest a detailed and complete JSON recipes. If the input is not an ingredient or  does not specify a complete ingredient, indicate that the input is not relevant to valid ingredient.
        Your task is to craft unique and delicious recipes using only these ingredients. Do not suggest any recipes that include additional or unspecified ingredients.
        There must not be no duplicate keys.
        For each recipe, provide the following details:
        1. Recipe Name(must exactly match the input if it's a known dish name)
        2. Category (Minimum 5 e.g., beverages, desserts)
        3. Nutrition Per Serving (format as JSON)
        4. Step-by-step Instructions(format as a JSON array, without any extra backslashes)
        5. Short Summary of Health Benefits(format as a JSON array, without any extra backslashes)
        6. Ratings (format as JSON,only numbers,must be out of 5)
        7. Short Description(format as a JSON array, within 15 words, without any extra backslashes)
        8. Duration
        9. Image URL (do not generate)

        Input Ingredients: {ingredients}
        
        Provide your response in JSON format only.Do not generate output that is not in properly formatted JSON. The format should be a JSON object like {{"items": [{{"name": "", "desc": ""}}]}}.Make sure there are no newline characters in the JSON object response.
        responses must be with above 10 keys "recipes1", "recipes2", "recipes3", "recipes4", "recipes5", ..., "recipes10" with each recipe containing:
        "name", "category", "Nutrition_Per_Serving", "instructions", "health_benefits", "Ratings", "Short_Description", "Duration", "image_url".

        Example format:
        
        request :
        {
            "input":["cinnamon","latte","sugar","fresh cow milk"]
        }
        response :
        {
            "recipes1": {
                "name": "Cinnamon Coffee Latte",
                "category": ["Beverages", "Hot Drinks"],
                "Nutrition_Per_Serving": {
                    "calories": "120",
                    "fat": "3g",
                    "cholesterol": "10mg",
                    "sodium": "100mg",
                    "protein": "2g"
                },
                "instructions": [
                    "Heat milk in a saucepan until steaming.",
                    "Stir in cinnamon powder and sugar.",
                    "Add instant coffee and whisk until frothy.",
                    "Serve hot."
                ],
                "health_benefits": [
                    "Rich in antioxidants from cinnamon.",
                    "Provides a quick energy boost."
                ],
                "Ratings": "3",
                "Short_Description": "A warm, spiced latte with aromatic cinnamon and rich coffee.",
                "Duration": "5 minutes",
                "image_url": "https://example.com/recipe.jpg"
            },
            ...
            "recipes10": {
            "name": "Cinnamon Sugar Milk",
            "category": [
                "Beverages",
                "Hot Drinks"
            ],
            "Nutrition_Per_Serving": {
                "calories": "100",
                "fat": "3g",
                "cholesterol": "10mg",
                "sodium": "50mg",
                "protein": "2g",
                "carbohydrates": "15g",
                "sugar": "10g"
            },
            "instructions": [
                "Heat fresh cow milk in a saucepan over medium heat.",
                "Add a pinch of cinnamon powder and a tablespoon of sugar to the milk.",
                "Stir until the sugar dissolves and the milk is heated through.",
                "Pour the milk into a mug and enjoy."
            ],
            "health_benefits": [
                "Cinnamon may help regulate blood sugar and aid in digestion.",
                "Milk is a good source of calcium and protein."
            ],
            "Ratings": "3",
            "Short_Description": "A simple and comforting drink with a hint of sweetness and spice.",
            "Duration": "3 minutes",
            "image_url": "https://example.com/recipe.jpg"
            }
        **Important Constraints**:
        - Recipes must strictly use only the provided ingredients.
        - Ensure no Key's fields are left empty for any recipe.
        - Avoid repetition in recipe suggestions.
        - Do not create recipes if the input list is empty or lacks valid ingredients replace with "enter valid ingredients".
        }

        If the input is not related to valid ingredient name, respond with:
            request :
            {
                "input":["america","latte","jfak282","fresh cow milk"]
            }
            response :
            {
                    "output": "It looks like your input isn't specific ingredient, Please provide an valid ingredient"
            }
        """
    
        #Gemini API
        genai.configure(api_key=os.getenv("GOOGLE_API_KEY"))
        model = genai.GenerativeModel('gemini-1.5-flash-latest')
        
        
        #prompt template with ingredients
        prompt = prompt_template.replace("{ingredients}", ', '.join(ingredients))
        response = model.generate_content(prompt)

        if not response or not response.text:
            logging.error("Failed to generate recipes: No response from the AI model.")
            return jsonify({"output": "Failed to generate recipes."}), 500

        response_text = response.text.replace('\n', '')
        logging.info("AI Response: %s", response_text)
        print(response.text)
        try:
            response_json = json.loads(response_text)
        except json.JSONDecodeError as e:
            logging.error("JSON Decode Error: %s",e)
            return jsonify({"error": "The response from the AI model is not in the expected format."}), 500

        if any(key.startswith("recipes") for key in response_json):
            # Generate image URLs for each recipe
            for recipe_key, recipe_details in response_json.items():
                dish_name = recipe_details["name"]
                image_url = get_image_url(dish_name)
                recipe_details["image_url"] = image_url
            # Insert recipe data into MongoDB
            try:
                result = collection.insert_one(response_json)
            except Exception as e:
                logging.error("MongoDB Insertion Error: %s", e)
                return jsonify({"error": "Failed to save recipes."}), 500

            recipe_id = str(result.inserted_id)

            formatted_response = {"recipe_id": recipe_id}
            for key, recipe in response_json.items():
                formatted_response[key] = recipe

            response_json.pop('_id', None)

            formatted_response = {
                "recipe_id": recipe_id,
                "recipes": response_json
            }

            return app.response_class(
                response=json_util.dumps(formatted_response),
                status=200,
                mimetype='application/json'
            )
        else:
            logging.warning("No valid recipes found for the given ingredients.")
            return jsonify({"output": "No valid recipes found for the given ingredients."}), 404

    except Exception as e:
        # Log error and return JSON response
        logging.error(str(e))
        return jsonify({"error": str(e)}), 500



@app.route('/get_recipes', methods=['POST'])
def get_recipes():
    try:
        data = request.get_json()
        user_input = data.get('input')

        if not user_input:
            return jsonify({"output": "Input is required"})

        prompt_template = """
        You are an intelligent assistant that provides recipe suggestions. If the input is a specific dish name , suggest a detailed and complete recipe. If the input is just an ingredient or regional style of cooking or does not specify a complete dish, indicate that the input is not relevant to food or recipes.

        Input: "{input_data}"

        For the recipe, it must to provide all the following details:
        1. Recipe Name(format as a JSON array, without any extra commas, without any extra backslashes , and must exactly match the input if it's a known dish name)
        2. List of Ingredients (format as a JSON array, without any extra commas, without any extra backslashes)
        3. Step-by-step Instructions (format as a JSON array, without any extra backslashes)
        4. Short Summary of Health Benefits for {input_data} (format as a JSON array, without any extra backslashes)

        Format the response as a JSON with key: "recipes". The "recipes" key should map to a single recipe object. The recipe object should have the following keys: "name", "ingredients", "instructions", and "health_benefits". Each key should be enclosed in double quotes, and each value should be a valid JSON data type.

        Example format:

        request
        {
        "input":"Chicken Fried Rice"
        }

        response
        {
            "recipes": {
                "name": "Chicken Fried Rice",
                "ingredients": [
                    "1 tablespoon vegetable oil",
                    "1 onion, chopped",
                    "2 cloves garlic, minced",
                    "1 cup cooked chicken, shredded or diced",
                    "1 (16 ounce) can mixed vegetables, drained",
                    "1/2 cup frozen peas",
                    "1/2 cup chopped carrots",
                    "1/4 cup soy sauce",
                    "1/4 cup chicken broth",
                    "2 cups cooked rice",
                    "1 egg, beaten (optional)"
                ],
                "instructions": [
                    "Heat the oil in a large skillet or wok over medium-high heat.",
                    "Add the onion and cook until softened, about 5 minutes.",
                    "Add the garlic and cook for 1 minute more.",
                    "Add the chicken, mixed vegetables, peas, and carrots to the skillet. Cook until the vegetables are heated through, about 5 minutes.",
                    "Stir in the soy sauce and chicken broth.",
                    "Add the rice and stir until combined.",
                    "Push the rice to the side of the skillet and pour in the egg. Cook the egg until scrambled, then stir it into the rice.",
                    "Serve immediately."
                ],
                "health_benefits": [
                    "Chicken fried rice is a good source of protein, carbohydrates, and vitamins.",
                    "The chicken provides protein, which is essential for building and repairing tissues.",
                    "The rice provides carbohydrates, which give you energy.",
                    "The vegetables provide vitamins and minerals, which are important for overall health."
                ]
            }
        }


        request
        {
        "input":"Lassi"
        }

        response
        {
            "recipes": {
                "name": "Lassi",
                "ingredients": [
                    "2 cups plain yogurt",
                    "3/4 cups cold water",
                    "1/2 teaspoon ground cumin",
                    "A pinch of salt",
                    "1 teaspoon sugar or honey (optional)"
                ],
                "instructions": [
                    "In a blender, combine yogurt, water, cumin, salt, and sugar (if using).",
                    "Puree until smooth and frothy.",
                    "Taste and adjust the seasonings to your preference.",
                    "Serve immediately or chill for later."
                ],
                "health_benefits": [
                    "Lassi is a refreshing and nutritious drink.",
                    "It is a good source of protein, calcium, and probiotics.",
                    "Probiotics are beneficial bacteria that support gut health.",
                    "Lassi can help improve digestion, boost immunity, and reduce inflammation."
                ]
            }
        }

        If the input is not related to food or recipes, respond with:
        
        response
        {
            "output": "It looks like your input isn't specific to a recipe or food item. Please provide an ingredient, dish name, or type of cuisine you'd like a recipe for"
        }
        """

        genai.configure(api_key=os.getenv("GEMINI_API_KEY"))
        model = genai.GenerativeModel('gemini-pro')
        prompt = prompt_template.replace("{input_data}", user_input)
        response = model.generate_content(prompt)
        print(response.text)
        if not response or not response.text:
            return jsonify({"output": "It looks like your input isn't specific to a recipe or food item. Please provide an ingredient, dish name, or type of cuisine you'd like a recipe for"})

        response_text = response.text.replace('\n', '')
        logging.info(response_text)
        
        try:
            response_json = json.loads(response_text)
        except json.JSONDecodeError as e:
            logging.error(e)
            return jsonify({"error": "The response from the Gemini model is not in the expected format. Please try again."})
            

        if "recipes" in response_json and response_json["recipes"]:
            result = collection.insert_one(response_json)
            id = str(result.inserted_id)

            formatted_response = {
                "recipe_id": id,
                "recipes": {
                    "name": response_json['recipes']['name'],
                    "ingredients": response_json['recipes']['ingredients'],
                    "instructions": response_json['recipes']['instructions'],
                    "health_benefits": response_json['recipes']['health_benefits']
                }
            }
            return jsonify(formatted_response)
        else:
            return jsonify({"output": "It looks like your input isn't specific to a recipe or food item. Please provide an ingredient, dish name, or type of cuisine you'd like a recipe for"})

    except Exception as e:
        return jsonify({"error": str(e)})
    
@app.route('/recipes', methods=['GET'])
def get_all_recipes():
    try:
        recipes = collection.find({}, {"_id": 1})
        recipe_ids = [str(recipe["_id"]) for recipe in recipes]
        return jsonify({"recipe_ids": recipe_ids})
    except Exception as e:
        return jsonify({"error": str(e)})
    
@app.route('/recipes/<recipe_id>', methods=['GET'])
def get_recipe_details(recipe_id):
    try:
        recipe = collection.find_one({"_id": ObjectId(recipe_id)})
        if recipe:
            recipe["_id"] = str(recipe["_id"])
            return jsonify({"recipe": recipe})
        else:
            return jsonify({"error": "Recipe not found"})
    except Exception as e:
        return jsonify({"error": str(e)})

if __name__ == '__main__':
    app.run(debug=True)
