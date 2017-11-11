#!/bin/bash

./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_984.xml   -g Run984_Merged.geo  -r 984,985,993,1005,1006,1007,1008,1009,1010,1011,1012,1015,1018,1040,1043,1061 
./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1154.xml  -g Run1154_Merged.geo -r 1154,1171,1189,1191,1205,1206,1210,1222,1226
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1319.xml  -g Run1319_Merged.geo -r 1319,1327,1341,1343,1345,1347,1351,1353,1355,1357,1358,1360,1361,1364,1366,1368,1369,1370,1371,1372,1373,1374,1375,1377,1381,1382,1383,1384
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1395.xml  -g Run1395_Merged.geo -r 1395,1399
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1518.xml  -g Run1526_Merged.geo -r 1518,1526,1548,1549
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1553.xml  -g Run1553_Merged.geo -r 1553,1554
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1563.xml  -g Run1563_Merged.geo -r 1563,1576,1577
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1578.xml  -g Run1578_Merged.geo -r 1578
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1597.xml  -g Run1597_Merged.geo -r 1597,1607,1608
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1790.xml  -g Run1790_Merged.geo -r 1790,1792
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1795.xml  -g Run1795_Merged.geo -r 1795,1797
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1799.xml  -g Run1799_Merged.geo -r 1799,1801
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1809.xml  -g Run1809_Merged.geo -r 1809
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1817.xml  -g Run1817_Merged.geo -r 1817
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1820.xml  -g Run1820_Merged.geo -r 1820,1821,1823
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1825.xml  -g Run1825_Merged.geo -r 1825,1829,1832
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1833.xml  -g Run1833_Merged.geo -r 1833,1835,1837 
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1839.xml  -g Run1839_Merged.geo -r 1839,1841
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1843.xml  -g Run1843_Merged.geo -r 1843,1845 
#./Megascript.py  -d -f Simple --chewieconfiguration chewiexml_dependent_2017April/2017April_ChewieConfiguration_Period_1854.xml  -g Run1854_Merged.geo -r 1854 

#1405, 1578 and 1854 geometry correction is not needed according to Siew's analysis runs. So skipped for now.
# 1518, 1806, 1818 have 0 entry. tried couple of times. not working. Solved. by changing the geometric run.  
# no root file associated with run 1812. There are no files in that range. "Are you sure you copied them with the right permissions?"
 
# x position = 31546
# y position = 23656.9 (may be around this point)

#
