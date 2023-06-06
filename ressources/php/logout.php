<?php

  if($_SERVER["REQUEST_METHOD"] != "POST") {
    echo "method not allowed";
    return;
  }

session_start();

if(isset($_SESSION["username"]))
  session_destroy();
header('Location: /php/login.php');
