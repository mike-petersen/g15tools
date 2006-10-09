#!/usr/bin/perl -w

use strict;
use DCOP::Amarok::Player;
use threads;
use threads::shared;

my $user = "$ENV{USER}";
my $player = DCOP::Amarok::Player->new( user => $user ) or die "Couldn't Attach DCOP Interface: $!\n";
my $pipe = "$ENV{HOME}/.g15amaroklcdpipe";

my $mknod = system("mknod $pipe p");

my $G15Cpid : shared = open(G15COMPOSER, "| g15composer $pipe");

my $vol : shared = $player->getVolume;
my $status  :  shared = ( $player->status() > 1 ) ? 1 : 0;

open(PIPE, ">>$pipe");
print PIPE "FL 0 10 \"/usr/share/fonts/ttf-bitstream-vera/Vera.ttf\"\n";

sub volUpdate {
	print PIPE "PC 0\n";
	print PIPE "TO 0 10 2 1 \"Volume\"\n";
	print PIPE "DB 3 27 157 35 2 $vol 100 3\n";
}

while($status == 0) {
	$status = ( $player->status() > 1 ) ? 1 : 0;
	sleep 1;
}

my $artist : shared = $player->artist;
my $title : shared = $player->title;
my $album : shared = $player->album;
my $trackTotalSecs : shared = $player->totaltimesecs;
my $trackCurSecs : shared = $player->trackCurrentTime;
my $trackTotalTime : shared = $player->totalTime;
my $trackCurTime : shared = $player->currentTime;

&initScreen();

$SIG{TERM} = \&bye;

my $progressThread = threads->create(\&progress);
$progressThread->detach();

while(1) {
	$_ = <STDIN>;
	chomp $_;
	if ( /trackChange/ ) {
		$artist = $player->artist;
		$title = $player->title;
		$album = $player->album;
		$trackTotalSecs = $player->totaltimesecs;
		$trackTotalTime = $player->totalTime;
		$trackCurSecs = $player->trackCurrentTime;
		$trackCurTime = $player->currentTime;
		initScreen();
		$artist = $player->artist;
		$title = $player->title;
		$album = $player->album;
		$trackTotalSecs = $player->totaltimesecs;
		$trackTotalTime = $player->totalTime;
		$trackCurSecs = $player->trackCurrentTime;
		$trackCurTime = $player->currentTime;
		initScreen();
	} elsif ( /engineStateChange/ ) {
                if( /playing/ ) {
                        $status = 1;
			initScreen();
                } elsif ( /pause/ ) {
                        $status = 0;
                } else {
                        $status = 0;
                }
        } elsif( /volumeChange/ ){
                m/: (\d+)/;
                $vol = $1;
		$status = 0;
		&volUpdate();
		$status = 1;
        }
	exit if( /exit/ );
	&bye() if( /kill/ );
}

sub initScreen {
	print PIPE "PC 0\n";
	print PIPE "DR 0 0 159 43 1 1\n";
	print PIPE "DR 3 22 157 40 1 0\n";
	print PIPE "PB 3 22 157 24 0\n";
	print PIPE "FP 0 15 0 0 0 1 \"$artist\"\n";
	print PIPE "FP 0 9 0 15 0 1 \"$title\"\n";
	print PIPE "DB 3 27 157 35 2 $trackCurSecs $trackTotalSecs 3\n";
	print PIPE "TO 0 35 0 1 \"    $trackCurTime / $trackTotalTime    \"\n";
}

sub progress {
	while(1) {
		if($status > 0) {
			$trackCurSecs = $player->trackCurrentTime;
			$trackCurTime = $player->currentTime;
			print PIPE "PB 5 27 155 35 0 1 1\n";
			print PIPE "DB 3 27 157 35 2 $trackCurSecs $trackTotalSecs 3\n";
			print PIPE "TO 0 35 0 1 \"    $trackCurTime / $trackTotalTime    \"\n";
		} elsif($status == -1) {
			return;
		}
		sleep 1;
	}
}
	
sub bye {
	$status = -1;
	sleep 2;
	print PIPE "PC 0\n";
	system("kill 15 $G15Cpid");
	close(PIPE);
	close(G15COMPOSER);
	$mknod = system("rm $pipe");
	exit 0;
}
