# manoj-gupta.github.io

## Install Jekyll on Ubuntu

Follow steps at [official website](https://jekyllrb.com/docs/installation/ubuntu/)

## Download repo

```
git clone https://github.com/manoj-gupta/manoj-gupta.github.io.git
```

## Install dependencies

```
sudo apt-get install ruby-full build-essential zlib1g-dev
```

## Export variable and update path

```
echo '# Install Ruby Gems to ~/gems' >> ~/.bashrc
echo 'export GEM_HOME="$HOME/gems"' >> ~/.bashrc
echo 'export PATH="$HOME/gems/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

## Install Jekyll and Bundler

```
gem install jekyll bundler
```

## Install gems and update bundle

```
gem install github-pages
bundle update
```

## Run server

```
bundle exec jekyll serve
```

Browse to http://localhost:4000
