# Token Edit for Qt 5

## Notable Features

### QAbstractItemModel Interface

The token edit is populated using the QAbstractItemModel interface. The widget reacts to changes from outside using the model’s signals.

### Usage of QStyle and QPalette

The implementation utilises the application’s style wherever possible. Colors are taken from the widget’s palette.

### Auto-Completion and validation

The token edit exposes its QLineEdit so that auto-completion and validation can easily be added as normally.

### Drag and Drop

Drag and Drop is implemented using the QAbstractItemModel interface. However only move operations are currently supported.

### High-DPI Support

The widget is properly scaled at different DPI scalings.

### Optional Collapsed State

When the widget is not in focus, tokens that do not fit into the widget without scrolling can be collapsed into one displaying the number of collapsed ones.

### Option to Remove Tokens

The token edit offers an intuitive way to delete item through both mouse and keyboard interaction.

