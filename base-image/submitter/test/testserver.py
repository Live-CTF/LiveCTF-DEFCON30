#!/usr/bin/env python3
from flask import Flask, request, jsonify

app = Flask(__name__)

@app.route("/flag", methods=['POST'])
def flag():
    content = request.json
    print(content)
    return "Congratulations!"
