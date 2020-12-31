from flask import render_template, url_for, flash, redirect, request, abort, Blueprint
from flask_login import current_user, login_required
from flaskblog import db
from flaskblog.models import Post
from flaskblog.posts.forms import PostForm

posts = Blueprint("posts", __name__)


@posts.route("/post/new", methods=["GET", "POST"])
@login_required
def new_post():
    form = PostForm()
    if form.validate_on_submit():
        post = Post(
            title=form.title.data, content=form.content.data, author=current_user
        )
        db.session.add(post)
        db.session.commit()
        flash("Your post has been created!", "success")
        return redirect(url_for("main.home"))
    return render_template(
        "create_post.html", title="New Post", form=form, legend="New Post"
    )


@posts.route("/post/<int:post_id>")
def post(post_id):
    post = Post.query.get_or_404(post_id)
    return render_template("post.html", title=post.title, post=post)


@posts.route("/post/<int:post_id>/update", methods=["GET", "POST"])
@login_required
def update_post(post_id):
    post = Post.query.get_or_404(post_id)
    if post.author != current_user:
        abort(403)
    form = PostForm()
    if form.validate_on_submit():
        post.title = form.title.data
        post.content = form.content.data
        db.session.commit()
        flash("Your post has been updated!", "success")
        return redirect(url_for("posts.post", post_id=post.id))
    elif request.method == "GET":
        form.title.data = post.title
        form.content.data = post.content
    return render_template(
        "create_post.html", title="Update Post", form=form, legend="Update Post"
    )


@posts.route("/post/<int:post_id>/delete", methods=["POST"])
@login_required
def delete_post(post_id):
    post = Post.query.get_or_404(post_id)
    if post.author != current_user:
        abort(403)
    db.session.delete(post)
    db.session.commit()
    flash("Your post has been deleted!", "success")
    return redirect(url_for("main.home"))


######## start - add dummy posts to the blog ########
# this is part of part 9, to add pagination to the blog
import json

# put this in your routes.py
# then add "import json" in routes.py
# save the json in static folder with the name "posts.json"
# then got to localhost:5000/debug_add_posts
# warning: make sure you already have 2 users before doing this!
@posts.route("/debug_add_posts")
def debug_add_post():
    json_path = os.path.join(app.root_path, "static", "posts.json")
    with open(json_path) as json_file:
        data = json.load(json_file)
        for post_data in data:
            author = User.query.get(post_data["user_id"])
            post = Post(
                title=post_data["title"], content=post_data["content"], author=author
            )
            db.session.add(post)
            db.session.commit()
    flash("Posts have been added!", "success")
    return redirect(url_for("main.home"))


######## end - add dummy posts to the blog ########
