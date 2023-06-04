print("Content-Type: text/html")    # HTML is following
print() 

print('''<!DOCTYPE html>
<html lang="en">
  <head>
    <title>mo7arrik x</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <form method="post" action="post.py">
      <input type="text" name="name" value=""> 
      <input type="number" name="age" value=""> 
      <input type="submit" value="send"> 
    </form>
  </body>
</html>''')
