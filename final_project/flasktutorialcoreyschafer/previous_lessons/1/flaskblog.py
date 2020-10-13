from flask import Flask, escape, request
from datetime import datetime

app = Flask(__name__)


@app.route('/home')
@app.route('/')
def hello():

    # to test, visit http://127.0.0.1:5000/?name=jorge

    # one way to do it:
    # if 'name' in request.args:
    #     name = request.args['name']
    # else:
    #     name = 'World'

    # other way, the correct one
    name1 = request.args.get("name", "World")
    return """
         <html><body>
             <h1>Hello, {name3}!</h1>
             The time is {thetime}.
         </body></html>
         """.format(
        name3=name1, thetime=str(datetime.now()))


@app.route('/about')
def about():
    name = request.args.get("name", "World")
    return "<h1>about page</h1>"


# by adding these 2 lines I don't need to run "flask run", just call the script normally
if __name__ == '__main__':
    app.run(debug=True)


# but if we don't want to use the 2 last above, we can trigger the script with "flask run"
# REM on CMD:
# cd "C:\Users\asd\Desktop\tests\CS50\final_project"
# set FLASK_APP=flaskblog.py
# set FLASK_DEBUG=1 # this is for being able to change the code without needing to restart the server
# flask run
#
# # on Powershell:
# cd "C:\Users\asd\Desktop\tests\CS50\final_project"
# $env:FLASK_APP = "flaskblog.py"
# $env:FLASK_ENV = "development"
# flask run
#
