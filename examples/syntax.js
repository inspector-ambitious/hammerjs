var f, line, content = '';

if (system.args.length !== 2) {
    system.exit(-1);
}

f = fs.open(system.args[1], 'r');
while (true) {
    line = f.readLine();
    if (line.length === 0) {
        break;
    }
    content += line;
}
f.close();

system.print(JSON.stringify(Reflect.parse(content), undefined, 4));
