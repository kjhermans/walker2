#!/usr/bin/perl

##
## Turns (parts of) a picture into a bitmap, usable by C.
##

use GD;

my ($picture, $dimension, $xoff, $yoff) = (undef, 1<<7, 0, 0);
my $img;

if (scalar(@ARGV)) { $picture = shift @ARGV; }
if (scalar(@ARGV)) { $dimension = shift @ARGV; }
if (scalar(@ARGV)) { $xoff = shift @ARGV; }
if (scalar(@ARGV)) { $yoff = shift @ARGV; }

open IMG, "< $picture" || die "Could not open image file $picture.";

if ($picture =~ /\.jpe?g$/i) {
  $img = GD::Image->newFromJpeg(\*IMG) || die "Could not decode JPEG";
}

print "#define IMGWIDTH $dimension\n";
print "#define IMGHEIGHT $dimension\n";
print "#define IMG { \\\n";
for (my $i=0; $i < $dimension; $i++) {
  print "  { /* Row $i */ \\\n";
  for (my $j=0; $j < $dimension; $j++) {
    my $index = $img->getPixel($xoff + $j, $yoff + $i);
    my ($r,$g,$b) = $img->rgb($index);
    print "    { $r, $g, $b, 0xff }";
    if ($j < $dimension-1) {
      print ",";
    }
    print " \\\n";
  }
  print "  }";
  if ($i < $dimension-1) {
    printf ",";
  }
  print " \\\n";
}
print "}\n";

close IMG;

exit 0;

1;
