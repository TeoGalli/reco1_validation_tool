# reco1_validation_tool
Macro for making the comparison of variables obtained with the GenRecoValidator analyzer

These macros can be used to produce plots comparing the varibles of two different simulation productions (usually one like reference and one to validate). These varibles are obtained through the larsoft GenRecoValidator analyzer (https://github.com/TeoGalli/duneana/tree/develop/duneana/GenRecoValidator). 
The files needed for comparison are 3:
run_GenRecoValidator.fcl is the fcl to be given as input to launch the analyzer.
RecoPlots.C marco to produce histograms of the analyzed varibles.
Reco1CompareDataDistribution.C macro to get the graphical comparison of the varibles between two different production bench.

First, we obtain the varibles of interest using an analyzer
 lar -c run_GenRecoValidator.fcl -s <production_reco_file.root>.

We get analysisOutput.root, a file that contains a tree with the varibles we want to observe.

We use RecoPlots.C to get root files with only the histograms inside to compare:

 root 'RecoPlots.C("<file_name.root>","<path/tree>")'
 
You get reco1_validation_histos.root, file that contains the histograms of the varibles.

You can use Reco1CompareDataDistribution.C to compare the obtained histograms against a reference output.

 root 'Reco1CompareDataDistribution.C("<comparison_histogram_file.root>","<sample_histogram_file.root>")'
