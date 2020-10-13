from flask import Flask, render_template, url_for

app = Flask(__name__)

posts = [
    {
        'author': 'Corey Schafer',
        'title': 'Blog Post 1',
        'content': 'First post content',
        'date_posted': 'April, 20, 2018'
    },
    {
        'author': 'Jane Doe',
        'title': 'Blog Post 2',
        'content': 'First post content',
        'date_posted': 'April, 21, 2018'
    }
]


@app.route('/home')
@app.route('/')
def hello():
    return render_template("home.html", posts=posts)


@app.route('/about')
def about():
    return render_template("about.html", title = 'About')


# by adding these 2 lines I don't need to run "flask run", just call the script normally
if __name__ == '__main__':
    app.run(debug=True)
