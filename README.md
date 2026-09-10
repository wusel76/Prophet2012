I decided to make the Prophet 2012 editor and librarian for the Sequential Circuits SCI Samplers 2000 and 2002 open source.

Currently there are no build projects or scripts present. I and Bertil (RIP buddy!) have written this editor quite a while ago, and a lot of the toolchain has been lost. 

Main current issue is that there is no new Apple Silicon build available. 

Hopefully the project could be maintained by the community in order to have recent builds for recent platforms.

Support for these samplers and this software is available in the [Sequential Samplers Facebook group](https://www.facebook.com/groups/sequentialsamplers).

The entire GUI of the editor is based on wxWidgets-2.8.12. The GUI layout had been created with wxFormBuilder_v3.3.4 (I believe) – but there might be some hardcoded tweaks in the implementation.

Some parts were for testing purposes and have not been part in the released versions. So there is not really something cleaned up so far and I am going to provide everything as is.

I am releasing this software under the GNU Affero General Public License, version 3 (AGPL-3.0); see [LICENSE](LICENSE). My intent is that further releases and changes remain open source in accordance with this license. I also ask that further releases of this software or final product remain free of charge, without paywalls or payments. This is my wish rather than an additional license restriction: AGPL-3.0 permits charging for the software.
