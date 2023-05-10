#include "datasummary.h"

DataSummary::DataSummary()
{
  connect(com, &Communication::tempChanged, this, &DataSummary::setTemperature);
  connect(com, &Communication::mvChanged, this, &DataSummary::setMV);
  connect(com, &Communication::svChanged, this, &DataSummary::setSV);
}

double DataSummary::getTemperature() const
{
    return temperature_;
}

double DataSummary::getMV() const
{
    return mv_;
}

double DataSummary::getSV() const
{
    return sv_;
}

void DataSummary::setTemperature(double temperature)
{
    if (temperature_ != temperature) {
        temperature_ = temperature;
        emit temperatureChanged(temperature_);
    }
}

void DataSummary::setMV(double mv)
{
    if (mv_ != mv) {
        mv_ = mv;
        emit mvChanged(mv_);
    }
}

void DataSummary::setSV(double sv)
{
    if (sv_ != sv) {
        sv_ = sv;
        emit svChanged(sv_);
    }
}
