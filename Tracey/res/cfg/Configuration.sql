CREATE TABLE Application
(
  Version TEXT NOT NULL
);

CREATE TABLE MainWindow
(
  Top INTEGER NOT NULL,
  Left INTEGER NOT NULL,
  Width INTEGER NOT NULL,
  Height INTEGER NOT NULL,
  Maximized INTEGER NOT NULL,
  SplitterState BLOB NOT NULL,
  CurrentTraceWindowIndex INTEGER NOT NULL DEFAULT -1
);

CREATE TABLE SourceHistory
(
  Item TEXT NOT NULL,
  Sort INTEGER NOT NULL
);

CREATE TABLE HighlightingGroup
(
  Id TEXT PRIMARY KEY,
  Name TEXT NOT NULL
);

CREATE TABLE HighlightingRule
(
  Id TEXT PRIMARY KEY,
  HighlightingGroupId TEXT NOT NULL,
  Item TEXT NOT NULL,
  Enabled INTEGER NOT NULL,
  MatchCase INTEGER NOT NULL,
  WholeWord INTEGER NOT NULL,
  ForegroundColor TEXT NOT NULL,
  BackgroundColor TEXT NOT NULL,
  Sort INTEGER NOT NULL,
  FOREIGN KEY(HighlightingGroupId) REFERENCES HighlightingGroup(Id) ON DELETE CASCADE
);

CREATE TABLE Trace
(
  Id TEXT PRIMARY KEY,
  Name TEXT NOT NULL,
  Position INTEGER NOT NULL,
  Source TEXT NOT NULL,
  RangeType INTEGER NOT NULL,
  OpenStart TEXT,
  OpenEnd TEXT,
  LineHandling INTEGER NOT NULL,
  LineSize TEXT,
  PreloadContents INTEGER NOT NULL,
  HighlightingGroupId TEXT,
  Autoscroll INTEGER NOT NULL,
  CurrentIndex INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE Filter
(
  Id TEXT PRIMARY KEY,
  TraceId TEXT NOT NULL,
  Name TEXT NOT NULL,
  Position INTEGER NOT NULL,
  SearchType INTEGER NOT NULL,
  MatchCase INTEGER NOT NULL,
  WholeWord INTEGER NOT NULL,
  SourceLines INTEGER NOT NULL,
  ExtraLinesBefore INTEGER NOT NULL,
  ExtraLinesAfter INTEGER NOT NULL,
  Expression TEXT NOT NULL,
  HighlightingGroupId TEXT,
  Autoscroll INTEGER NOT NULL,
  FOREIGN KEY(TraceId) REFERENCES Trace(Id) ON DELETE CASCADE
);

CREATE TABLE OperatorPosition
(
  FilterId TEXT NOT NULL,
  Position INTEGER NOT NULL,
  FOREIGN KEY(FilterId) REFERENCES Filter(Id) ON DELETE CASCADE
);

CREATE TABLE SearchInfo
(
  SearchType INTEGER NOT NULL,
  MatchCase INTEGER NOT NULL,
  WholeWord INTEGER NOT NULL,
  GoTo TEXT
);

CREATE TABLE SearchHistory
(
  Item TEXT NOT NULL,
  Sort INTEGER NOT NULL
);

CREATE TABLE Settings
(
  Id TEXT PRIMARY KEY,
  Value TEXT NOT NULL
);

CREATE TABLE Shortcuts
(
  Id INTEGER PRIMARY KEY,
  Value TEXT NOT NULL,
  Description TEXT NOT NULL
);


INSERT INTO Application (Version)
  VALUES ('v0.9');
  
INSERT INTO MainWindow (Top, Left, Width, Height, Maximized, SplitterState)
  VALUES (-6666, -6666, 1100, 750, 1, "\x00\xFF\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x01\x90\x00\x00\x05\xD8\x01\xFF\xFF\xFF\xFF\x01\x00\x00\x00\x01\x00");

INSERT INTO SearchInfo (SearchType, MatchCase, WholeWord, GoTo)
  VALUES (0, 0, 0, null);

INSERT INTO Settings (Id, Value)
  VALUES ('minimizeToTray', '0');
INSERT INTO Settings (Id, Value)
  VALUES ('closeToTray', '0');
INSERT INTO Settings (Id, Value)
  VALUES ('deleteTrace', '1');
INSERT INTO Settings (Id, Value)
  VALUES ('deleteFilter', '1');
INSERT INTO Settings (Id, Value)
  VALUES ('deleteSourceHistory', '1');
INSERT INTO Settings (Id, Value)
  VALUES ('deleteSearchHistory', '1');
INSERT INTO Settings (Id, Value)
  VALUES ('deleteHighlightingGroup', '1');
INSERT INTO Settings (Id, Value)
  VALUES ('deleteHighlightingRule', '1');
INSERT INTO Settings (Id, Value)
  VALUES ('exitApplication', '1');

INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (0, 'Ctrl+Shift+T', 'Add Trace');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (1, 'Ctrl+Shift+F', 'Add Filter');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (2, 'Ctrl+D', 'Clear');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (3, 'Ctrl+S', 'Save');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (4, 'Ctrl+F', 'Find');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (5, 'F3', 'Find Next');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (6, 'Ctrl+F3', 'Find Previous');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (7, 'Ctrl+F2', 'Toggle Mark');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (8, 'F2', 'Next Mark');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (9, 'Shift+F2', 'Previous Mark');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (10, 'Ctrl+Shift+A', 'Autoscroll');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (11, 'Ctrl+Shift+W', 'Delete Trace');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (12, 'Ctrl+W', 'Delete Filter');
INSERT INTO Shortcuts (Id, Value, Description)
  VALUES (13, 'Ctrl+Return', 'Start/Stop');