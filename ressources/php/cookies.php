<?php
if($_SERVER["REQUEST_METHOD"] == "POST") {
  if(!empty($_POST['key']) && !empty($_POST['value']))
    setcookie($_POST['key'], $_POST['value']);
  header("refresh:0");
}
?>
<!DOCTYPE html>
<html lang="en">
  <head>
    <title>test post request</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>

</html>
    <form method="post">
      <input type="text" name="key" value="">
      <input type="text" name="value" value="">
      <input type="submit" name="" value="submit">
    </form>
<ul>  
  <?php
  if(count($_COOKIE) > 0) {
  foreach($_COOKIE as $key => $value)
    echo "<li><b>key</b>: ". $key . " <b>value</b>: " . $value . "</li>";
  } else {

    echo "<li>empty</li>";
  }
  ?>
</ul>
</body>

  
