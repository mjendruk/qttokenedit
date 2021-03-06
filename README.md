_Please note: This project and its site on Github are still under development._



# Qt Token Edit

Implementation of a token edit based on the Qt model/view framework. 



![Preview](images/tokenedit_preview.gif)


## Requirements

* C++14 compliant compiler 
* Qt version >= 5.9 (a lower version may work but was not tested)




## Notable Features

### QAbstractItemModel Interface

The token edit is populated using the QAbstractItemModel interface. The widget reacts to changes from outside using the model’s signals.



### Usage of QStyle and QPalette

The implementation utilises the application’s style wherever possible. Colors are taken from the widget’s palette.



__MacOS__

<a target="_blank" href="images/styles/macos.png">
  <img src="images/styles/macos_1x.png"  alt="MacOS Style"/>
</a>


__Windows__

<a target="_blank" href="images/styles/windows_10.png">
  <img src="images/styles/windows_10_1x.png"  alt="Windows Style"/>
</a>

__Gnome Adwaita__

<a target="_blank" rel="noopener noreferrer" href="images/styles/gnome_adwaita.png">
  <img src="images/styles/gnome_adwaita_1x.png"  alt="Gnome Adwaita Style"/>
</a>

### Auto-Completion and validation

The token edit exposes its QLineEdit so that auto-completion and validation can easily be added as normally.



### Drag and Drop

Drag and Drop is implemented using the QAbstractItemModel interface. However only move operations are currently supported.



### High-DPI Support

The widget is properly scaled at different DPI scale factors.



### Optional Collapsed State

When the widget is not in focus, tokens that do not fit into the widget without scrolling can be collapsed into one displaying the number of collapsed ones.



### Option to Remove Tokens

The token edit offers an intuitive way to delete items through both mouse and keyboard interaction.



## Known Issues / Todos

- [ ] Only move operations are supported for drag and drop.
- [ ] The widget does not rescale when moving between windows with difference DPI scaling
- [ ] Add unit tests for the testable parts of the widget
- [ ] Implement a Qt Designer plugin


## References

- Token fields in MacOS [[link]](https://developer.apple.com/design/human-interface-guidelines/macos/fields-and-labels/token-fields/)
- Input chips in Material Design [[link]](https://material.io/components/chips/#input-chips)
