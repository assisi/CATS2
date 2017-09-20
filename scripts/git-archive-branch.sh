#Attention: this script doesn't make any checks on where the branch has been 
#merged to HEAD, so use it with caution!
git tag archive/$1 $1
git branch -d $1
git push origin :$1