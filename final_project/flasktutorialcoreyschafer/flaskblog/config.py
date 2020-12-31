
class Config:
    SECRET_KEY = "dcf8c557345a5b3481a481aaad5192d5"
    SQLALCHEMY_DATABASE_URI = "sqlite:///site.db"
    #### - start - gMail settings ####
    MAIL_SERVER = 'smtp.googlemail.com'
    # # if the above doesn't work, use:
    # MAIL_SERVER = 'smtp.gmail.com'
    # # Either use TLS:
    # MAIL_PORT = 587
    # MAIL_USE_TLS = True
    # # Or SSL:
    MAIL_PORT = 465
    MAIL_USE_SSL = True
    # # Either will need you to set to enable Less Protected Apps in Gmail Account Security
    # # https://myaccount.google.com/security?gar=1
    MAIL_USERNAME = 'xxxxx@gmail.com'
    MAIL_PASSWORD = 'xxxxx'
    # #### - end - gMail settings ####

    #### - start - other mail settings ####MAIL_SERVER = 'mail.xxxxxx.com.ar'

    # Either use TLS:
    MAIL_PORT = 587
    MAIL_USE_TLS = True
    # Or SSL: MAIL_PORT = 465 MAIL_USE_SSL = True MAIL_USERNAME = 'xxxxxx' MAIL_PASSWORD = 'xxxxxx'
    ### - end - other mail settings ####


    # For security reasons, these 4 variables should be environment variables, then you can use:
    # import os
    # MAIL_USERNAME = os.environ.get('EMAIL_USER')
    # MAIL_PASSWORD = os.environ.get('EMAIL_PASS')
    # SECRET_KEY = os.environ.get('SECRET_KEY')
    # SQLALCHEMY_DATABASE_URI = os.environ.get('SQLALCHEMY_DATABASE_URI')
