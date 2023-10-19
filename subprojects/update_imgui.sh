#!/usr/bin/bash

# Clone docking branch of DearImGUI
[ -d imgui_docking ] && rm -rf imgui_docking
git clone --branch docking --single-branch --depth 1 https://github.com/ocornut/imgui imgui_docking

# Clone ImPlot
[ -d implot_new ] && rm -rf implot_new
git clone --depth 1 https://github.com/epezent/implot implot_new

#
# Merge file differences
#

sources=("c" "cpp" "h" "hpp")
dirs_diff=`diff -qr -x backends -x docs -x examples -x ".*" imgui_docking imgui`
new_files=`echo -n "$dirs_diff" | grep "Only in imgui_docking" | sed 's@Only in imgui_docking\(.*\): \(.*\)@imgui_docking\1/\2 imgui\1/\2@g'`
changed_files=`echo -n "$dirs_diff" | grep "Files imgui_docking" | sed 's@Files \(.*\) and \(.*\) differ@\1 \2@g'`

echo "----- NEW FILES -----"
echo -n "$new_files" | awk '{print $1}'
echo "----- CHANGED FILES -----"
echo -n "$changed_files" | awk '{print $1 " ~~ " $2}'

# Update new files
while IFS= read -r f; do
  n=`echo $f | awk '{print $1}'`
  e="${n##*.}"
  if [[ " ${sources[@]} " =~ " $e " ]]; then
    echo "NEW SOURCE FILE: $f"
  else
    if [ "$f" != "" ]; then
      eval "cp -r $f"
    fi
  fi
done <<< "$new_files"

# Update changed files
while IFS= read -r f; do
  if [ "$f" != "" ]; then
    eval "cp $f"
  fi
done <<< "$changed_files"

# Copy backends
while IFS= read -r line; do
  if [ "$line" != "" ]; then
    eval "cp $line"
  fi
done <<< `diff -qr imgui_docking imgui | grep -E "Files.*backends" | sed 's@Files \(.*\) and \(.*\) differ@\1 \2@'`

# Update meson.bulid git commit
cd imgui_docking
commit=`git rev-parse HEAD`
cd ../imgui
sed -i "s/git-.*'/git-$commit'/" meson.build
cd ..

rm -rf imgui_docking


# Update implot
cp -r implot_new/* implot/
rm -rf implot_new
