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
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
  </head>
  <body>

</html>
  <div class="container mt-4">
    <form method="post">
    <div class="row">
      <div class="col-6">
      <input class="form-control" type="text" name="key" value="">
      </div>
      <div class="col-6">
      <input  class="form-control" type="text" name="value" value="">
      </div>
      <input type="submit" name="" class="btn btn-primary mt-4"  value="submit">
    </div>
    </form>
  </div>


<div class="container">

<table class="table mt-4">
  <thead>
    <tr>
      <th scope="col">#</th>
      <th scope="col">key</th>
      <th scope="col">value</th>
      <th scope="col">Action</th>
    </tr>
  </thead>
  <tbody>


  <?php
      $i = 1;
      foreach($_COOKIE as $key => $value) {
        echo '<tr><th scope="row">'. $i .'</th>';
        echo '<td>'. $key.'</td>';
        echo '<td>'. $value.'</td>';
        echo '<td><a href="clearcookies.php?key='. $key.'">delete</a></td>';
      $i++;
      }
  ?>
    </tr>
  </tbody>
  </table>
</div>
<ul class>  
</ul>
</body>

  
