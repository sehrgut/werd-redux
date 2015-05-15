#!/usr/bin/perl
#
# Random words from simple rules (see datafiles); recursive;
# info on datafile format can be found in the English rulset;
# by Chris Pound, pound@rice.edu, after Mark Rosenfelder's Pascal version
#
srand(time);
$usage="usage: $0 -number file\n";
$ARGV[0]=~/^-(\d+)$/ && ($numb = $1) || die $usage;
$file=$ARGV[1] || die $usage;

open(DATA, $file) || die "$file not found\n";
while (<DATA>) { /^([A-Z]):(.*)\s+$/; $rule{$1} = $2; }

for (1..$numb) { print &parse('W'),"\n"; }

sub parse {
 $_=pop(@_);
 /[^A-Z]/ && return $_;
 @TMP=split(/\s+/, $rule{$_});
 $_=splice(@TMP, rand @TMP, 1);
 for (split('')) { print &parse($_); }
}
