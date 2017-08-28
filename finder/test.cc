// set_union example
#include <iostream>     // std::cout
#include <algorithm>    // std::set_union, std::sort
#include <vector>       // std::vector
using namespace std;

int main () 
{
  vector<string> f;
  vector<string> s;
  
  f.push_back("#include \"mainwindow.h\""                                  );  s.push_back("#include \"mainwindow.h\""                                  );
  f.push_back("#include \"ui_mainwindow.h\""                               );  s.push_back("#include \"ui_mainwindow.h\""                               );
  f.push_back("//=========================================================");  s.push_back("//=========================================================");
  f.push_back("MainWindow::MainWindow (QWidget *parent   ) :"              );  s.push_back("MainWindow::MainWindow (QWidget *parent   ) :"              );
  f.push_back("            QMainWindow(parent            ) ,"              );  s.push_back("            QMainWindow(parent            ) ,"              );
  f.push_back("            ui_        (new Ui::MainWindow)"                );  s.push_back("            ui_        (new Ui::MainWindow)"                );
  f.push_back("{"                                                          );  s.push_back("{"                                                          );
  f.push_back("    ui_ ->setupUi      (this              ) ;"              );  s.push_back("    ui_ ->setupUi      (this              ) ;"              );
  f.push_back(""                                                           );  s.push_back(""                                                           );
  f.push_back("    char* homePath = getenv (\"HOME\"  );"                  );  s.push_back("    char* homePath = getenv (\"HOME\"  );"                  );
  f.push_back("    HOME_          = QString(homePath);"                    );  s.push_back("alfa  HOME_TOWN          = QString(homePath);"              );
  f.push_back("    qsettingPath_  = HOME_ + QString(\"/.config/finder/\");");  s.push_back("    qsettingPath_  = HOME_ + QString(\"/.config/finder/\");");
  f.push_back(""                                                           );  s.push_back(""                                                           );
  f.push_back("    this->initializeStringFinder() ;"                       );  s.push_back("    this->initializeStringFinder() ;"                       );
  f.push_back(""                                                           );  s.push_back(""                                                           );
  f.push_back("    fileModel_[\"left\" ] = new QFileSystemModel() ;"       );  s.push_back("    fileModel_[\"left\" ] = new QFileSystemModel() ;"       );
  f.push_back("    fileModel_[\"right\"] = new QFileSystemModel() ;"       );  s.push_back("}"                                                          ); 
  f.push_back("}"                                                          );  s.push_back(""                                                           );
  f.push_back(""                                                           );  s.push_back(""                                                           );
  f.push_back(""                                                           );  s.push_back("  vector<string> v(10);"                                    );
  f.push_back("  vector<string> v(10);"                                    );  s.push_back("  vector<string>::iterator itu;"                            );
  f.push_back("  vector<string>::iterator itu;"                            );  s.push_back("  vector<string>::iterator itddd;"                          );
  f.push_back("  vector<string>::iterator itf;"                            );  s.push_back("  vector<string>::iterator itf;"                            );
  f.push_back("  vector<string>::iterator its;"                            );  s.push_back("  vector<string>::iterator its;"                            );

  std::vector<string> v(100);
  std::vector<string>::iterator itu;
  std::vector<string>::iterator itf;
  std::vector<string>::iterator its;

  itu=std::set_union (f.begin(), f.end(), s.begin(), s.end(), v.begin());
                                               
  v.resize(itu-v.begin());                     

  cout << "The union has " << (v.size()) << " elements:\n";
  for (itu=v.begin(); itu!=v.end(); ++itu)
  {
    itf = ::find(f.begin(), f.end(), *itu) ;
    its = ::find(s.begin(), s.end(), *itu) ;
    if(      itf != f.end() && its != s.end())
    {
      cout << *itf << " - " << *its << endl ;
    }
    else if( itf == f.end() && its != s.end())
    {
      cout << "  " << " - " << *its << endl ;
    }
    else if( itf != f.end() && its == s.end())
    {
      cout << *itf << " - " << "  " << endl ;
    }
  }
  cout << '\n';

  return 0;
}
