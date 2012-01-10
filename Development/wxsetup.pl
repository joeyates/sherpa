=pod

This script automates the downloading and setup of
a compatible wxWidgets version for Sherpa development.

Run as follows:
>perl wxsetup.pl

=cut

use strict;
use LWP::Simple;

###########################################################
# Settings

my $sWxWidgetsSourceFilename = 'wxWidgets-2.6.2.tar.bz2';
my $sSourceUrl = 'http://biolpc22.york.ac.uk/pub/2.6.2/wxWidgets-2.6.2.tar.bz2';
my $sSourceTreeRoot = 'wxWidgets-2.6.2';
my $sBuildType = 'gtk2'; # also used as the name of the build directory

###########################################################
# Predeclarations

sub IsWxWidgetsInstalled();
sub DownloadwxWidgetsSource();
sub Configure();
sub Make();

###########################################################
# Main program

# Don't buffer output
local $| = 1;

my $sPath = `pwd`;
$sPath =~ s/[\r\n]+$//go;

print <<EOT;
$0

This script will download, compile and install a suitable version
of the wxWidgets libraries (if they are not already available).

EOT

my $bIsWxWidgetsInstalled = IsWxWidgetsInstalled();

exit
	if($bIsWxWidgetsInstalled);

if(!-e $sWxWidgetsSourceFilename)
	{
	DownloadwxWidgetsSource();
	}
print "Source code tarball is '$sPath/$sWxWidgetsSourceFilename'\n";

if(!-d $sSourceTreeRoot)
	{
	print "Uncompressing wxWidgets source to '$sSourceTreeRoot'\n";

	`tar -xkjf $sWxWidgetsSourceFilename`;
	}
print "Source code tree is '$sPath/$sSourceTreeRoot/'\n";

chdir $sSourceTreeRoot;

if(!-d $sBuildType)
	{
	mkdir $sBuildType;
	}
print "Build directory is '$sPath/$sSourceTreeRoot/$sBuildType/'\n";

chdir $sBuildType;

Configure();

Make();

print "Done!\n";

##################################################################################
# Subroutines

=pod

Returns true if a wxWidgets version >= 2.6.2 is installed
with a unicode build

=cut

sub IsWxWidgetsInstalled()
	{
	# See if wx-config exists
	my $sWxConfig = `which wx-config`;
	
	return 0
		if($sWxConfig !~ /(\/[a-zA-Z\s_\.]+)+\/wx-config/o);
	
	#print "'wx-config' is installed as $sWxConfig\n";
	
	my $sWxWidgetsVersions = `wx-config --list`;

	#print "Installed versions: $sWxWidgetsVersions\n";
	
	return 0
		if($sWxWidgetsVersions !~ /Default\sconfig\sis\s+([^\r\n]+)/o);
	
	my $sDefault = $1;
		
	if($sDefault !~ /-unicode/o)
		{
		print <<EOT;
You have wxWidgets installed, but your default set of libraries is
not a unicode build.

EOT
		
		return 0;
		}

	print <<EOT;
OK, wxWidgets unicode build is installed and is the default for wx-config.

There is nothing to do.

EOT
	
	return 1;
	}

sub DownloadwxWidgetsSource()
	{
	print "Downloading wxWidgets source from '$sSourceUrl'\n";

	my $rc = mirror($sSourceUrl, $sWxWidgetsSourceFilename);
	
	die("Can't download source file '$sSourceUrl'")
		if(is_error($rc));
	}

sub Configure()
	{
	print "Running 'configure':\n";
	#  --enable-unicode

	my $sConfigureCommand = <<EOT;
../configure --with-gtk \\
--enable-shared --enable-unicode \\
--enable-debug --enable-debug_flag --enable-debug_cntxt \\
--enable-exceptions \\
--enable-dynlib --enable-dynamicloader \\
--enable-xpm --enable-cmdline --with-gnomeprint --disable-metafile
EOT

	print "$sConfigureCommand\n\n";

	`$sConfigureCommand`;
	}

sub Make()
	{
	print "Running 'make':\n";
	`make`;

	print "Running 'make install':\n";
	print "Please type in your root password:\n";
	`su -c 'make install'`;
	}
