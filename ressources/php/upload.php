<?php

$target_dir = $_SERVER['UPLOAD_TMP_DIR'];
$file_name =  basename($_FILES["file"]["name"]);
$target_file = $target_dir . basename($_FILES["file"]["name"]);
$uploadOk = 1;
$imageFileType = strtolower(pathinfo($target_file, PATHINFO_EXTENSION));

// Check if image file is a actual image or fake image
if (isset($_POST["submit"])) {
  $check = getimagesize($_FILES["file"]["tmp_name"]);
  if ($check !== false) {
    echo "File is an image - " . $check["mime"] . ".";
    $uploadOk = 1;
  } else {
    echo "File is not an image.";
    $uploadOk = 0;
  }
}

// Check if $uploadOk is set to 0 by an error
if ($uploadOk == 0) {
  echo "Sorry, your file was not uploaded.";
  // if everything is ok, try to upload file
} else {
  if (move_uploaded_file($_FILES["file"]["tmp_name"], $target_file)) {
    // echo "The file " . htmlspecialchars(basename($_FILES["file"]["name"])) . " has been uploaded.";
  } else {
    echo "Sorry, there was an error uploading your file.";
  }
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
  <title></title>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link href="css/style.css" rel="stylesheet">
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-9ndCyUaIbzAi2FUVXJi0CjmCapSmO7SnpJef0486qhLnuZ2cdeRhO02iuK6FUUVM" crossorigin="anonymous">
  <style>
    @import url("https://fonts.googleapis.com/css?family=DM+Sans:400,500,700&display=swap");

    * {
      box-sizing: border-box;
    }

    html,
    body {
      margin: 0;
      padding: 0;
      width: 100%;
      height: 100%;
    }

    body {
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 30px 10px;
      font-family: "DM Sans", sans-serif;
      transition: background 0.4s ease-in;
      background-color: #c394f8;
    }

    body.blue {
      background-color: #428aa6;
    }

    input[type=radio] {
      display: none;
    }

    .card {
      position: absolute;
      width: 60%;
      height: 100%;
      left: 0;
      right: 0;
      margin: auto;
      transition: transform 0.4s ease;
      cursor: pointer;
    }

    .container {
      width: 100%;
      max-width: 800px;
      max-height: 600px;
      height: 100%;
      transform-style: preserve-3d;
      display: flex;
      justify-content: center;
      flex-direction: column;
      align-items: center;
    }

    .cards {
      position: relative;
      width: 100%;
      height: 100%;
      margin-bottom: 20px;
    }

    img {
      width: 100%;
      height: 100%;
      border-radius: 10px;
      -o-object-fit: cover;
      object-fit: cover;
    }

    #item-1:checked~.cards #song-3,
    #item-2:checked~.cards #song-1,
    #item-3:checked~.cards #song-2 {
      transform: translatex(-40%) scale(0.8);
      opacity: 0.4;
      z-index: 0;
    }

    #item-1:checked~.cards #song-2,
    #item-2:checked~.cards #song-3,
    #item-3:checked~.cards #song-1 {
      transform: translatex(40%) scale(0.8);
      opacity: 0.4;
      z-index: 0;
    }

    #item-1:checked~.cards #song-1,
    #item-2:checked~.cards #song-2,
    #item-3:checked~.cards #song-3 {
      transform: translatex(0) scale(1);
      opacity: 1;
      z-index: 1;
    }

    #item-1:checked~.cards #song-1 img,
    #item-2:checked~.cards #song-2 img,
    #item-3:checked~.cards #song-3 img {
      box-shadow: 0px 0px 5px 0px rgba(81, 81, 81, 0.47);
    }

    .player {
      background-color: #fff;
      border-radius: 8px;
      min-width: 320px;
      padding: 16px 10px;
    }

    .upper-part {
      position: relative;
      display: flex;
      align-items: center;
      margin-bottom: 12px;
      height: 36px;
      overflow: hidden;
    }

    .play-icon {
      margin-right: 10px;
    }

    .song-info {
      width: calc(100% - 32px);
      display: block;
    }

    .song-info .title {
      color: #403d40;
      font-size: 14px;
      line-height: 24px;
    }

    .sub-line {
      display: flex;
      justify-content: space-between;
      width: 100%;
    }

    .subtitle,
    .time {
      font-size: 12px;
      line-height: 16px;
      color: #c6c5c6;
    }

    .time {
      font-size: 12px;
      line-height: 16px;
      color: #a5a5a5;
      font-weight: 500;
      margin-left: auto;
    }

    .progress-bar {
      height: 3px;
      width: 100%;
      background-color: #e9efff;
      border-radius: 2px;
      overflow: hidden;
    }

    .progress {
      display: block;
      position: relative;
      width: 60%;
      height: 100%;
      background-color: #2992dc;
      border-radius: 6px;
    }

    .info-area {
      width: 100%;
      position: absolute;
      top: 0;
      left: 30px;
      transition: transform 0.4s ease-in;
    }

    #item-2:checked~.player #test {
      transform: translateY(0);
    }

    #item-2:checked~.player #test {
      transform: translateY(-40px);
    }

    #item-3:checked~.player #test {
      transform: translateY(-80px);
    }
  </style>
</head>

<body>
  <div class="container">
    <input type="radio" name="slider" id="item-1" checked>
    <input type="radio" name="slider" id="item-2">
    <input type="radio" name="slider" id="item-3">
    <div class="cards">
      <label class="card" for="item-1" id="song-1">
        <?php
        echo '<img src="../uploads/' . $file_name . '" alt="song">';
        ?>
      </label>
    </div>
    <div class="player">
      <div class="upper-part">
        <div class="info-area" id="test">
          <label class="song-info" id="song-info-1">
            <div class="title"></div>
            <?php
            if (isset($_POST["name"]))
              echo $_POST["name"];
            else
              echo "jhon doe";
            ?>
        </div>
      </div>
    </div>
        <div>
          <a href="upload.html" class="btn btn-primary mt-4">upload more</a>
        </div>
  </div>
</body>
<script>
</script>

</html>
