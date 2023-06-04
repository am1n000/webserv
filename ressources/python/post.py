print("Content-Type: text/html")    # HTML is following
print() 


import cgi
form = cgi.FieldStorage()


def pr():
    if "name" not in form or "age" not in form:
        print("<H1>Error</H1>")
        print("Please fill in the name and age fields.")
        return
    print("<p>name:", form["name"].value)
    print("<p>age:", form["age"].value)

pr()
