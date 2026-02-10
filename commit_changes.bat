@echo off
echo Adding all changes to git...
git add .

set /p message="Enter commit message: "
echo Committing changes...
git commit -m "%message%"

echo Pushing to GitHub...
git push

echo.
echo All changes have been committed and pushed to GitHub!
pause