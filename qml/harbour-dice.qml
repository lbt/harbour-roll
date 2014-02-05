import QtQuick 2.0
import Sailfish.Silica 1.0
import "pages"

ApplicationWindow
{
    _backgroundVisible : false
    initialPage: Component { DicePage { } }
    cover: undefined
    onApplicationActiveChanged: {
        // This exits the About page if it is open when the app is minimised
        if (pageStack.depth == 2) { pageStack.navigateBack(PageStackAction.Immediate); }
    }
}


