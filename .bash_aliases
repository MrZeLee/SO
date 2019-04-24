alias sshpi="ssh pi@mourahouse.ddns.net"
alias sshpc="ssh josel@mourahouse.ddns.net -p 23"
alias wakePC="wakeonlan 34:97:f6:82:37:ef"

alias svnli1="svn checkout svn://svn.alunos.di.uminho.pt/2018li1g055 --username 2018li1g055a2 --password sOVCUniesn"
alias agenda="gcalcli calw 2"
alias cd..="cd .."

alias open="gnome-open"
alias o="gnome-open"
alias p="pwd"
alias h="cd ~"

#GitHub

alias g="git log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit --date=relative"

#Shcool
#alias MIEI="cd ~/OneDrive/MIEI"
#alias MIEI1="cd /home/josel/OneDrive/MIEI/1_Ano"
#alias MIEI11="cd /home/josel/OneDrive/MIEI/1_Ano/1ºSemestre"

function m()
{
	foo=$1
	folder=$(ls ~ | grep OneDrive)

	if [ $# -eq 0 ]
	then
	  cd ~/"$folder"/MIEI;
	elif [ $# -eq 1 ] && [ ${#foo} -eq 1 ]
	then
	  if [ -d ~/"$folder"/MIEI/$1_Ano ]
	  then
	    cd ~/"$folder"/MIEI/$1_Ano;
	  else
	    echo "Folder doesn't exist."
	  fi
	else
          {
	  year=$(( $foo / 10));
	  sem=$(( $foo - (( $year * 10 )) ));
	  if [ -d ~/"$folder"/MIEI/"$year"_Ano/"$sem"_Semestre ]
	  then
	    cd ~/"$folder"/MIEI/"$year"_Ano/"$sem"_Semestre;
	  else
	    echo "Folder doesn't exist."
	  fi
	  }
	fi
}

#move along the folders
function cdl()
{
	if [ $# -eq 0 ]
	then cd .; ls;
	else cd $1; ls;
	fi
}

#search for name
function s { sudo find .| grep $1;}

#find in file
function fif()
{
    if [ $# -lt 2 ]
    then
        echo 'At least 2 arguments'
        exit 0
    else
	clear
        if [ $# -eq 2 ]
        then
            cat $1 | grep -i $2;
	else
	    array=( $@ )
            len=${#array[@]}
            _domain=${array[$len-1]}
            _args=${array[@]:0:$len-1}

            fif $_args | grep -i $_domain;
	fi
    fi
}

#alias for packages Linux
alias update='sudo apt-get update'
alias upgrade='sudo apt-get upgrade'
alias agi='sudo apt-get install'

#python -m SimpleHTTPServer [port]
alias shareFolder="python -m SimpleHTTPServer 1235"

#show shortcuts
alias shortcuts="cat ~/.bash_aliases"

#show external ip
alias externalip="curl ifconfig.me"

#prompt

# get current branch in git repo
function parse_git_branch() {
	BRANCH=`git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/'`
	if [ ! "${BRANCH}" == "" ]
	then
		STAT=`parse_git_dirty`
		echo "[${BRANCH}${STAT}]"
	else
		echo ""
	fi
}

# get current status of git repo
function parse_git_dirty {
	status=`git status 2>&1 | tee`
	dirty=`echo -n "${status}" 2> /dev/null | grep "modified:" &> /dev/null; echo "$?"`
	untracked=`echo -n "${status}" 2> /dev/null | grep "Untracked files" &> /dev/null; echo "$?"`
	ahead=`echo -n "${status}" 2> /dev/null | grep "Your branch is ahead of" &> /dev/null; echo "$?"`
	newfile=`echo -n "${status}" 2> /dev/null | grep "new file:" &> /dev/null; echo "$?"`
	renamed=`echo -n "${status}" 2> /dev/null | grep "renamed:" &> /dev/null; echo "$?"`
	deleted=`echo -n "${status}" 2> /dev/null | grep "deleted:" &> /dev/null; echo "$?"`
	bits=''
	if [ "${renamed}" == "0" ]; then
		bits=">${bits}"
	fi
	if [ "${ahead}" == "0" ]; then
		bits="*${bits}"
	fi
	if [ "${newfile}" == "0" ]; then
		bits="+${bits}"
	fi
	if [ "${untracked}" == "0" ]; then
		bits="?${bits}"
	fi
	if [ "${deleted}" == "0" ]; then
		bits="x${bits}"
	fi
	if [ "${dirty}" == "0" ]; then
		bits="!${bits}"
	fi
	if [ ! "${bits}" == "" ]; then
		echo " ${bits}"
	else
		echo ""
	fi
}

export PS1="\[\033[01;32m\][\A] \u@\h\[\033[00m\]:\[\033[01;34m\]\W\[\033[00m\]\[\033[01;31m\]\`parse_git_branch\`\[\033[00m\]\\$ "

#copie bash files to ~
alias upbash="sudo ~/Git/bash_aliases/upbash"

#Encrypt and Decrypt

function enc ()
{
	if [ $# -eq 0 ]
	then
	  echo "You have no arguments.";
	elif [ -f ./$1 ]
	then
	  openssl enc -aes-256-cbc -salt -in $1 -out $1.enc; echo "$1 encrypted.";
	else
	  echo "File doesn't exist.";
	fi
}

function dec ()
{
	if [ $# -eq 0 ]
	then
	  echo "You have no arguments.";
	elif [ -f ./$1.enc ]
	then
	  openssl enc -aes-256-cbc -d -in $1.enc -out $1; echo "$1 decrypted.";
	else
	  echo "File doesn't exist.";
	fi
}
