#include "EditorWindow.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    EditorWindow editor;
    editor.show();

    return a.exec();
}
