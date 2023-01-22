const path = require('path');
var copyWebpackPlugin = require('copy-webpack-plugin');

module.exports = {
  // the entry file for the bundle
  entry: path.join(__dirname, '/client/src/app.jsx'),

  // the bundle file we will get in the result
  output: {
    path: path.join(__dirname, '/client/dist/js'),
    filename: 'app.js',
  },

  plugins: [
    new copyWebpackPlugin({
      patterns: [
        { from: path.join(__dirname, '/node_modules/tinymce/plugins'),
            to: 'plugins' },
        { from: path.join(__dirname, '/node_modules/tinymce/themes'),
            to: 'themes' },
        { from: path.join(__dirname, '/node_modules/tinymce/skins'),
            to: 'skins' }
      ]
      //{ from: './node_modules/tinymce/plugins', to: './plugins' },
      //{ from: './node_modules/tinymce/themes', to: './themes' },
      //{ from: './node_modules/tinymce/skins', to: './skins' }
    })
  ],

  module: {

    // apply loaders to files that meet given conditions
    rules: [{
      test: /\.jsx?$/,
      include: path.join(__dirname, '/client/src'),
      loader: 'babel-loader',
      options: {
        presets: ['@babel/preset-env', '@babel/preset-react']
      }},
      {
        test: /\.css?$/,
        include: path.join(__dirname, '/client/css'),
        use: ['style-loader', 'css-loader']
      },
      {
        test: /\.css?$/,
        include: path.join(__dirname, './node_modules/reactflow/dist/'),
        use: ['style-loader', 'css-loader']
      },
      {
        test: /\.css?$/,
        include: path.join(__dirname, './node_modules/react-meter-bar/dist/'),
        use: ['style-loader', 'css-loader']
      }/*,
      {
        test: require.resolve('tinymce/tinymce'),
        loaders: [
            'imports-loader?this=>window',
            'exports-loader?window.tinymce'
        ]
      },
      {
          test: /tinymce\/(themes|plugins)\//,
          loaders: [
              'imports-loader?this=>window'
          ]
      }*/]
  },

  // start Webpack in a watch mode, so Webpack will rebuild the bundle on changes
  watch: true,
  mode: "production"
};
