import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached


@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    total = {}

    rowCash = db.execute(
        "SELECT cash FROM users WHERE id = :user_id", user_id=session["user_id"])

    total["cash"] = rowCash[0]["cash"]

    rows = db.execute(
        """   SELECT symbol
                   , SUM(shares) AS sumShares
                   , SUM(shares*price) AS TotPurchVal
                FROM transactions
               WHERE user_id = :user_id
            GROUP BY symbol
              HAVING SUM(shares) != 0
        """, user_id=session["user_id"])

    if not rows:
        flash("Portfolio is empty.")
        return render_template("index.html", rows=rows, total=total)

    total["stock"] = 0
    total["purch"] = 0

    # some row calculations snd formatting
    for row in rows:
        stock = lookup(row["symbol"])
        row["name"] = f'{stock["name"]} [{stock["symbol"]}]'
        row["currPrice"] = stock["price"]
        row["totalCurr"] = row['sumShares'] * stock["price"]
        row["diff"] = row["totalCurr"] - row['TotPurchVal']
        total["stock"] += row["totalCurr"]
        total["purch"] += row['TotPurchVal']

    # calculate totals
    total["assets"] = total["stock"] + total["cash"]
    total["earnings"] = total["stock"] - total["purch"]

    return render_template("index.html", rows=rows, total=total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        symbol = request.form.get("symbol").strip()
        shares = request.form.get("shares").strip()

        # Render apologies per exercise statement

        # Ensure symbol was submitted
        if not symbol:
            return apology("invalid symbol", "Error")

        # Ensure symbol was submitted or valid
        elif not shares.isdigit() or int(shares) < 1:
            return apology("invalid shares", "Error")

        sharesNum = int(shares)

        # Api call. Stock lookup
        quote = lookup(symbol)

        # ensure symbol exists
        if not quote:
            return apology("symbol not found", "Error")

        priceNum = float(quote["price"])

        # according to the "lookup" function defined in helpers.py
        # the quote variable is of the type dictionary, and after calling the API, 3 keys/values will be parsed: name, price and symbol
        # i.e we can call price by typing quote["price"]

        # calculate total cost
        finalPrice = priceNum * sharesNum

        # retrieve user's cash
        rows = db.execute(
            "SELECT cash FROM users WHERE id = :userId", userId=session["user_id"])

        # Cash in user's account
        usrCash = rows[0]["cash"]

        if finalPrice > usrCash:
            return apology(f"not enough cash ({usrCash})", "Error")

        # Update user's cash
        db.execute("UPDATE users SET cash = cash - :price WHERE id = :user_id",
                   price=finalPrice, user_id=session["user_id"])

        # Register transaction
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price) VALUES(:qUser_id, :qSymbol, :qShares, :qPrice)",
                   qUser_id=session["user_id"],
                   qSymbol=quote["symbol"],
                   qShares=sharesNum,
                   qPrice=priceNum)

        flash(
            f"{sharesNum} {quote['symbol']} share{'s' if sharesNum > 1 else ''} bought.")
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    rows = db.execute(
        """   SELECT symbol
                   , ABS(shares) AS shares
                   , price
                   , abs(shares) * price AS total
                   , CASE WHEN shares > 0 THEN 'Bought' ELSE 'Sold' END AS action
                   , created_at
                FROM transactions
               WHERE user_id = :user_id
            ORDER BY created_at DESC
        """,
        user_id=session["user_id"])

    return render_template("history.html", rows=rows)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Render apologies per exercise statement

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # the strip method trims leading and trailing spaces
        symbol = request.form.get("symbol").strip()

        # according to the "lookup" function defined in helpers.py
        # the quote variable is of the type dictionary, and after calling the API, 3 keys/values will be parsed: name, price and symbol
        # i.e we can call price by typing quote["price"]
        quote = lookup(symbol)

        if not quote:
            return apology("invalid symbol", "Error")

        # format text number to text currency
        quote["price"] = usd(quote["price"])

        # in quoted.html, we will need to call this variable as quoteIn instead of quote
        return render_template("quoted.html", quoteIn=quote)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # the logic below has been mostly ported from the login part which was provided by CS50

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Ensure username was submitted
        if not username:
            return apology("must provide username", "Error")

        # Ensure password was submitted
        elif not password:
            return apology("must provide password", "Error")

        # Ensure confirmation was submitted
        elif not confirmation:
            return apology("must provide confirmation", "Error")

        elif password != confirmation:
            return apology("Password and confirmation don't match", "Error")

        # Query database for username
        rows = db.execute(
            "SELECT id FROM users WHERE username = :qUsername", qUsername=username)

        # Ensure username doesn't exist
        if len(rows) != 0:
            return apology("username already exists", "Error")

        # password hash
        hash = generate_password_hash(password)

        # insert user
        # exercise hints:
        # If an INSERT or UPDATE would violate some constraint (e.g., a UNIQUE index), then execute returns None. In cases of error, execute raises a RuntimeError.
        # If str is an INSERT, and the table into which data was inserted contains an autoincrementing PRIMARY KEY, then execute returns the value of the newly inserted row’s primary key.
        newrowPk = db.execute(
            "INSERT INTO users (username, hash) VALUES(:qUsername, :qHash)", qUsername=username, qHash=hash)

        if newrowPk:

            # Remember which user has logged in
            session["user_id"] = newrowPk

            # Display a flash message
            flash("User has been registered.")

            # Redirect user to home page
            return redirect("/")

        else:
            return apology("Registration failed", "Error")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/change_password", methods=["GET", "POST"])
def change_password():
    """Change User Password"""

    # the logic below has been mostly ported from the register part

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        currPassword = request.form.get("currPassword")
        newPassword = request.form.get("newPassword")
        confPassword = request.form.get("confPassword")

        # Ensure currPassword was submitted
        if not currPassword:
            return apology("must provide currPassword", "Error")

        # Ensure newPassword was submitted
        elif not newPassword:
            return apology("must provide new password", "Error")

        # Ensure confPassword was submitted
        elif not confPassword:
            return apology("must provide confirmation", "Error")

        elif newPassword != confPassword:
            return apology("Password and confirmation don't match", "Error")

        # password hash
        hash = generate_password_hash(newPassword)

        # Update Password
        db.execute(
            "UPDATE users SET hash = :hash WHERE id = :user_id", user_id=session["user_id"], hash=hash)

        flash("Password has been changed.")

        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("change_password.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # the logic below was mostly ported from the buy part

        symbol = request.form.get("symbol").strip()
        shares = request.form.get("shares").strip()

        # Render apologies per exercise statement

        # Ensure symbol was submitted
        if not symbol:
            return apology("invalid symbol", "Error")

        # Ensure symbol was submitted or valid
        elif not shares.isdigit() or int(shares) < 1:
            return apology("invalid shares", "Error")

        sharesNum = int(shares)

        # Api call. Stock lookup
        quote = lookup(symbol)

        # ensure symbol exists
        if not quote:
            return apology("symbol not found", "Error")

        priceNum = float(quote["price"])

        # according to the "lookup" function defined in helpers.py
        # the quote variable is of the type dictionary, and after calling the API, 3 keys/values will be parsed: name, price and symbol
        # i.e we can call price by typing quote["price"]

        # retrieve user's shares
        rows = db.execute(
            """SELECT SUM(shares) AS sumShares
                 FROM transactions
                WHERE user_id = :user_id
                  AND symbol = :symbol
             GROUP BY symbol
            """,
            user_id=session["user_id"],
            symbol=quote["symbol"])

        if not rows:
            return apology(f"no {quote['symbol']} shares in your Portfolio", "Error")

        # Shares in user's account
        usrShares = rows[0]["sumShares"]

        if sharesNum > usrShares:
            return apology(f"not enough shares ({usrShares})", "Error")

        # calculate total earnings
        finalPrice = priceNum * sharesNum

        # Update user's cash
        db.execute("UPDATE users SET cash = cash + :price WHERE id = :user_id",
                   price=finalPrice, user_id=session["user_id"])

        # Register transaction
        db.execute("INSERT INTO transactions (user_id, symbol, shares, price) VALUES(:qUser_id, :qSymbol, :qShares, :qPrice)",
                   qUser_id=session["user_id"],
                   qSymbol=quote["symbol"],
                   qShares=-sharesNum,
                   qPrice=priceNum)

        flash(
            f"{sharesNum} {quote['symbol']} share{'s' if sharesNum > 1 else ''} sold.")
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        rows = db.execute(
            """SELECT symbol
                    , SUM(shares) AS sumShares
                 FROM transactions
                WHERE user_id = :user_id
             GROUP BY symbol
               HAVING SUM(shares) != 0
            """, user_id=session["user_id"])
        if not rows:
            flash("Portfolio is empty.")

        return render_template("sell.html", rows=rows)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
