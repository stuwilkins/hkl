#include "angleconfiguration.h"

namespace hkl {
  
AngleConfiguration::AngleConfiguration(void)
{
}

AngleConfiguration::AngleConfiguration(AngleConfiguration const & aC)
{
  m_axeMap = aC.m_axeMap;
  m_samples = aC.m_samples;
  m_detectors = aC.m_detectors;
}

AngleConfiguration::~AngleConfiguration(void)
{
}

bool
AngleConfiguration::operator==(AngleConfiguration const & aC) const
{
  return m_axeMap == aC.m_axeMap
          && m_samples == aC.m_samples
          && m_detectors == aC.m_detectors;
}

std::ostream &
AngleConfiguration::printToStream(std::ostream & flux) const
{
  int nb_axes = m_samples.size();
  int i;
  
  flux << "Samples: (" << nb_axes << ")" << std::endl;
  for(i=0; i<nb_axes; i++)
    flux << "  " << m_axeMap[m_samples[i]];

  nb_axes = m_detectors.size();
  flux << "Detectors: (" << nb_axes << ")" << std::endl;
  for(i=0; i<nb_axes; i++)
    flux << "  " << m_axeMap[m_detectors[i]];
  flux << std::endl;
  
  return flux;
}

std::vector<std::string> const
AngleConfiguration::getAxesNames(void) const
{
  std::vector<std::string> nameList;

  AxeMap::const_iterator iter = m_axeMap.begin();
  AxeMap::const_iterator end = m_axeMap.end();
  while(iter != end){
    nameList.push_back(iter->first);
    iter++;
  }
  
  return nameList;
}

Axe &
AngleConfiguration::operator[](std::string const & name) throw (HKLException)
{
  try{
    return m_axeMap[name];
  } catch (HKLException const &){
    throw;
  }
}

Axe const &
AngleConfiguration::operator[](std::string const & name) const throw (HKLException)
{
  try{
    return m_axeMap[name];
  } catch (HKLException const &){
    throw;
  }
}

void
AngleConfiguration::addSampleAxe(Axe const & axe) throw (HKLException)
{
  std::string const & name = axe.get_name();
  
  //Est-ce que cet axe est deja present dans la liste?
  std::vector<std::string>::iterator sample_iter = m_samples.begin();
  std::vector<std::string>::iterator sample_end = m_samples.end();
  while(sample_iter != sample_end){
    if (*sample_iter == name)
      throw HKLException("Can not add two times the same axe",
                         "change the name of the axe",
                         "AngleConfiguration::addSampleAxe");
    sample_iter++;
  }
  
  AxeMap::iterator iter = m_axeMap.find(name);
  AxeMap::iterator end = m_axeMap.end();

  if (iter == end){
    m_axeMap.insert(AxeMap::value_type(name, axe));
    m_samples.push_back(name);
  } else {
    if (iter->second == axe){
      m_samples.push_back(name);
    } else {
      throw HKLException("Can not add this axe",
                         "Same name but different axe",
                         "AngleConfiguration::addSampleAxe");
    }
  }
}

void
AngleConfiguration::addDetectorAxe(Axe const & axe) throw (HKLException)
{
  std::string const & name = axe.get_name();

  //Est-ce que cet axe est deja present dans la liste?
  std::vector<std::string>::iterator detector_iter = m_detectors.begin();
  std::vector<std::string>::iterator detector_end = m_detectors.end();
  while(detector_iter != detector_end){
    if (*detector_iter == name)
      throw HKLException("Can not add two times the same axe",
                         "change the name of the axe",
                         "AngleConfiguration::addDetectorAxe");
    detector_iter++;
  }

  AxeMap::iterator iter = m_axeMap.find(name);
  AxeMap::iterator end = m_axeMap.end();

  if (iter == end){
    m_axeMap.insert(AxeMap::value_type(name, axe));
    m_detectors.push_back(name);
  } else {
    if (iter->second == axe){
      m_detectors.push_back(name);
    } else {
      throw HKLException("Can not add this axe",
                         "Same name but different axe",
                         "AngleConfiguration::addDetectorAxe");
    }
  }
}

smatrix
AngleConfiguration::getSampleRotationMatrix(void) const
{
  int nb_axes = m_samples.size();
  Quaternion q;
  
  for(int i=0;i<nb_axes;i++)
    q *= m_axeMap[m_samples[i]].asQuaternion();
  
  return q.asMatrix();
}

svector
AngleConfiguration::getQ(Quaternion const & qi) const
{
  // Attention pour l'instant qf est obtenu a partir de qi
  // il faudrait prendre 1, 0, 0 comme référence.
  int nb_axes = m_detectors.size();
  Quaternion qr;

  for(int i=0;i<nb_axes;i++)
    qr *= m_axeMap[m_detectors[i]].asQuaternion();
  
  Quaternion q(qr);
  q *= qi;
  q *= qr.conjugate();
  q -= qi;

  return svector(q[1], q[2], q[3]);
}

} // namespace hkl

std::ostream& operator<< (std::ostream& flux, hkl::AngleConfiguration const & aC)
{
  return aC.printToStream(flux);
}