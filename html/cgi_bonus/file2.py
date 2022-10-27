#!/usr/bin/python3
# coding=utf-8

# Import modules for CGI handling 
import cgi, cgitb 
cgitb.enable()

# Create instance of FieldStorage 
form = cgi.FieldStorage() 

# Get data from fields
first_name = form.getvalue('first_name')
last_name  = form.getvalue('last_name')

header = "Content-Type: text/html\r\n\r\n"

html = """
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>Hello - Second CGI Program</title>
</head>
<body>
  <h2>Hello {0} {1}</h2>
</body>
</html>
""".format(first_name, last_name)

print(header + html)
