FROM python:3.10.14-slim

# Set environment variables
ENV PYTHONDONTWRITEBYTECODE 1
ENV PYTHONUNBUFFERED 1

# Install system dependencies
RUN apt-get update && apt-get install -y \
    gcc \
    libpq-dev \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory in the container
WORKDIR /app

# Copy and install requirements
COPY ./requirements.txt /app/
RUN pip install --no-cache-dir -r requirements.txt
RUN pip install gunicorn

# Copy the application code into the container
COPY ./ /app/

# Expose port 5000 for Flask and Gunicorn
EXPOSE 5010

# Command to run the Flask application using Gunicorn
CMD ["gunicorn", "-b", "0.0.0.0:5010", "-c", "gunicorn_config.py", "main:app"] 