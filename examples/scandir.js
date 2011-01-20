if (system.args.length !== 2) {
    system.exit(-1);
}

var scanDirectory = function (path) {
    if (fs.exists(path) && fs.isFile(path) && path.match('.js$')) {
        system.print(path);
    } else if (fs.isDirectory(path)) {
        fs.list(path).forEach(function (e) {
            scanDirectory(path + '/' + e);
        });
    }
};

scanDirectory(system.args[1]);
