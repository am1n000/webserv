use strict;
use warnings;
use CGI;

my $cgi = CGI->new;

if($ENV{'REQUEST_METHOD'} ne 'POST') {
print $cgi->header( -type => 'text/plain' );
  print "unsupported method, supported methods, POST";
  return;
}
my %param = map { $_ => scalar $cgi->param($_) } $cgi->param() ;
print $cgi->header( -type => 'text/plain' );
print "you have been submitted a form with post requst with this data\n";
for my $k ( sort keys %param ) {
    print join ": ", $k, $param{$k};
    print "\n";
}
