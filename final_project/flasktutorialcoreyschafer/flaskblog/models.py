from flaskblog import db
from datetime import datetime


class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(20), unique=True, nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    image_file = db.Column(db.String(20), nullable=False, default="default.jpg")
    password = db.Column(db.String(60), nullable=False)
    posts = db.relationship("Post", backref="author", lazy=True)

    def __repr__(self):
        return f"User('{self.username}', '{self.username}','{self.image_file}')"


class Post(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    title = db.Column(db.String(100), nullable=False)
    date_posted = db.Column(db.DateTime, nullable=False, default=datetime.utcnow)
    content = db.Column(db.Text, nullable=False)
    user_id = db.Column(db.Integer, db.ForeignKey("user.id"), nullable=False)

    def __repr__(self):
        return f"Post('{self.title}', '{self.username}','{self.date_posted}')"


# now with all the code above, we can create the db, to do it:
# run in the console's project:
#    python
#    from flaskblog import db
#    db.create_all()
# site.db should appear in the root project's folder
#
# to create a user
# from flaskblog import User, Post
# user_1 = User(username='Corey', email='C@demo.com', password='password')
# db.session.add(user_1)
# user_2 = User(username='JohnDoe', email='j@demo.com', password='password')
# db.session.add(user_2)
# db.session.commit()
