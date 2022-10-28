<html>
 <head>
  <title>PHP Test</title>
 </head>
 <body>
 <?php echo '<p>Hello World</p>'; ?> 
 <?php
    if ($_GET['user'] != "") {
        echo "<p>Your Username is {$_GET['user']}</p>";
    }
    if ($_GET['password'] != "") {
        echo "<p>Your password is {$_GET['password']}</p>";
    }
?>
</body>
</html>