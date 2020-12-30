from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_bcrypt import Bcrypt
from flask_login import LoginManager
from flask_mail import Mail

app = Flask(__name__)
app.config["SECRET_KEY"] = "dcf8c557345a5b3481a481aaad5192d5"
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///site.db"
db = SQLAlchemy(app)
bcrypt = Bcrypt(app)
login_manager = LoginManager(app)
login_manager.login_view = "login"
login_manager.login_message_category = "info"

#### - start - gMail settings ####
# app.config['MAIL_SERVER'] = 'smtp.googlemail.com'
# # if the above doesn't work, use:
# # app.config['MAIL_SERVER'] = 'smtp.gmail.com'
# # Either use TLS:
# # app.config['MAIL_PORT'] = 587
# # app.config['MAIL_USE_TLS'] = True
# # Or SSL:
# app.config['MAIL_PORT'] = 465
# app.config['MAIL_USE_SSL'] = True
# # Either will need you to set to enable Less Protected Apps in Gmail Account Security
# # https://myaccount.google.com/security?gar=1
# app.config['MAIL_USERNAME'] = 'xxxxx@gmail.com'
# app.config['MAIL_PASSWORD'] = 'xxxxx'
# #### - end - gMail settings ####

#### - start - other mail settings ####
app.config['MAIL_SERVER'] = 'mail.xxxxxx.com.ar'

# Either use TLS:
# app.config['MAIL_PORT'] = 587
# app.config['MAIL_USE_TLS'] = True
# Or SSL:
app.config['MAIL_PORT'] = 465
app.config['MAIL_USE_SSL'] = True
app.config['MAIL_USERNAME'] = 'xxxxxx'
app.config['MAIL_PASSWORD'] = 'xxxxxx'
### - end - other mail settings ####


# if setting the user and password as environment variables, then you can use:
# import os
# app.config['MAIL_USERNAME'] = os.environ.get('EMAIL_USER')
# app.config['MAIL_PASSWORD'] = os.environ.get('EMAIL_PASS')
mail = Mail(app)


# this import is needed here and not at first part in order to avoid circular import issue
from flaskblog import routes