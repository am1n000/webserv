#include <cstdio>

int main ()
{
  if( std::remove( "uploads" ) != 0 )
    perror( "Error deleting file" );
  else
    puts( "File successfully deleted" );
  return 0;
}