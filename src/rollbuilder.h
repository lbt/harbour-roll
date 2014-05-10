#ifndef ROLLBUILDER_H
#define ROLLBUILDER_H

#include "worldbuilder.h"
#include "rollworld.h"
#include <QList>
#include "track.h"

class RollBuilder : public WorldBuilder
{
    Q_OBJECT
public:
    explicit RollBuilder(RollWorld *parent = 0);
    void setup();
    QStringList getTrackNames();

signals:

public slots:
    bool loadTracks(QString jsonFile);
    void setTrack(QString trackname);

protected:
    RollWorld* m_rollworld;
    QMap<QString, Track*> m_tracks;
    QString m_currentTrack;
};

#endif // ROLLBUILDER_H
