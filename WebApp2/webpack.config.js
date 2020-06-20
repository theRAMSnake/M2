const path = require('path');


module.exports = {
  // the entry file for the bundle
  entry: path.join(__dirname, '/client/src/app.jsx'),

  // the bundle file we will get in the result
  output: {
    path: path.join(__dirname, '/client/dist/js'),
    filename: 'app.js',
  },

  module: {

    // apply loaders to files that meet given conditions
    rules: [{
      test: /\.jsx?$/,
      include: path.join(__dirname, '/client/src'),
      loader: 'babel-loader',
      query: {
        presets: ['@babel/preset-env', '@babel/preset-react']
      }},
      {
        test: /\.css?$/,
        include: path.join(__dirname, '/client/css'),
        use: ['style-loader', 'css-loader']
      }]
  },

  // start Webpack in a watch mode, so Webpack will rebuild the bundle on changes
  watch: true,
  mode: "production"
};
