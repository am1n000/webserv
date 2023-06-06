<?php

session_start();

if(!isset($_SESSION["username"])) {
  header('Location: /php/login.php');
} else {
?>



<!DOCTYPE html>
<html lang="en">
  <head>
    <title>test post request</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <h1>Dashboard</h1>   
    <p>you are logged in and this is dashboard</p>
    <form method="post" action="logout.php">
      <input type="submit" value="logout">
    </form>
  </body>
</html>

<?php
} 
?>
