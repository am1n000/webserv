use strict;
use warnings;
use CGI;
my $cgi = CGI->new();
print $cgi->header;


if($ENV{'REQUEST_METHOD'} ne 'GET') {
  print "unsupported method for this route, supported method (GET)";
  return;
}
print <<'END';
<!doctype html>
<html lang="en">
  <head>
    <title>mo7arrik x</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
  </head>
  <body>
    <h1> this is a form from perl cgi</h1>
    <p> when you submit this form you will make a post requst to an end point that will be executed by perl cgi</p>
    <form method="post" action="post.pl">
      <input type="text" name="name" value=""> 
      <input type="number" name="age" value=""> 
      <input type="submit" value="send"> 
    </form>
  </body>
END
