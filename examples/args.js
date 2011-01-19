if (system.args.length === 1) {
    system.print('Try to pass some args when invoking this script!');
} else {
    system.args.forEach(function (arg, i) {
        system.print(i + ': ' + arg);
    });
}
system.exit();
