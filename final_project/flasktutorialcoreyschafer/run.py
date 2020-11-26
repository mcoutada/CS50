from flaskblog import app

# by adding these 2 lines I don't need to run "flask run", just call the script normally
if __name__ == "__main__":
    app.run(debug=True)
