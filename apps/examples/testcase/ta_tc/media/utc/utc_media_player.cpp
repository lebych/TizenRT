/****************************************************************************
 *
 * Copyright 2018 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific
 * language governing permissions and limitations under the License.
 *
 ****************************************************************************/

#include <stdio.h>
#include <media/MediaPlayer.h>
#include <media/FileInputDataSource.h>

#define MEDIAPLAYER_TEST_FILE "/mnt/utc_media_player.raw"

class TestObserver : public media::MediaPlayerObserverInterface
{
public:
	int value[3];
	TestObserver() : value{0, 0, 0} {}
	void onPlaybackStarted(Id id) override;
	void onPlaybackFinished(Id id) override;
	void onPlaybackError(Id id) override;
};

void TestObserver::onPlaybackStarted(Id id)
{
	value[0] = 1;
}

void TestObserver::onPlaybackFinished(Id id)
{
	value[1] = 2;
}

void TestObserver::onPlaybackError(Id id)
{
	value[2] = 3;
}

class SimpleMediaPlayerTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		fp = fopen(MEDIAPLAYER_TEST_FILE, "w");
		fputs("dummyfile", fp);
		fclose(fp);
		source = std::move(std::unique_ptr<media::stream::FileInputDataSource>(
			new media::stream::FileInputDataSource(MEDIAPLAYER_TEST_FILE)));
		source->setSampleRate(16000);
		source->setChannels(2);
		mp = new media::MediaPlayer();
	}
	virtual void TearDown()
	{
		remove(MEDIAPLAYER_TEST_FILE);
		delete mp;
	}

	FILE* fp;
	std::unique_ptr<media::stream::FileInputDataSource> source;
	media::MediaPlayer* mp;
};

TEST_F(SimpleMediaPlayerTest, create)
{
	EXPECT_EQ(mp->create(), media::PLAYER_OK);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, createTwice)
{
	mp->create();

	EXPECT_EQ(mp->create(), media::PLAYER_ERROR);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, destroy)
{
	mp->create();

	EXPECT_EQ(mp->destroy(), media::PLAYER_OK);
}

TEST_F(SimpleMediaPlayerTest, destroyTwice)
{
	mp->create();
	mp->destroy();

	EXPECT_EQ(mp->destroy(), media::PLAYER_ERROR);
}

TEST_F(SimpleMediaPlayerTest, setDataSource)
{
	mp->create();

	EXPECT_EQ(mp->setDataSource(std::move(source)), media::PLAYER_OK);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, setDataSourceWithNullptr)
{
	mp->create();

	EXPECT_EQ(mp->setDataSource(nullptr), media::PLAYER_ERROR);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, setObserver)
{
	auto observer = std::make_shared<TestObserver>();
	mp->create();
	mp->setDataSource(std::move(source));
	mp->setObserver(observer);
	mp->prepare();
	mp->pause();
	mp->start();
	mp->unprepare();
	mp->destroy();
	EXPECT_EQ(observer->value[0], 1);
	EXPECT_EQ(observer->value[1], 2);
	EXPECT_EQ(observer->value[2], 3);
}

TEST_F(SimpleMediaPlayerTest, prepare)
{
	mp->create();
	mp->setDataSource(std::move(source));

	EXPECT_EQ(mp->prepare(), media::PLAYER_OK);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, prepareTwice)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	EXPECT_EQ(mp->prepare(), media::PLAYER_ERROR);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, prepareWithoutDataSource)
{
	mp->create();

	EXPECT_EQ(mp->prepare(), media::PLAYER_ERROR);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, unprepare)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	EXPECT_EQ(mp->unprepare(), media::PLAYER_OK);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, unprepareTwice)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();
	mp->unprepare();

	EXPECT_EQ(mp->unprepare(), media::PLAYER_ERROR);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, start)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	EXPECT_EQ(mp->start(), media::PLAYER_OK);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, startWithoutPrepare)
{
	mp->create();
	mp->setDataSource(std::move(source));

	EXPECT_EQ(mp->start(), media::PLAYER_ERROR);

	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, pause)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();
	mp->start();

	EXPECT_EQ(mp->pause(), media::PLAYER_OK);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, pauseWithoutPrepare)
{
	mp->create();
	mp->setDataSource(std::move(source));

	EXPECT_EQ(mp->pause(), media::PLAYER_ERROR);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, stop)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();
	mp->start();

	EXPECT_EQ(mp->stop(), media::PLAYER_OK);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, stopWithoutPrepare)
{
	mp->create();
	mp->setDataSource(std::move(source));

	EXPECT_EQ(mp->stop(), media::PLAYER_ERROR);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, setVolume)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	for (int vol = 0; vol <= 31; vol++) {
		EXPECT_EQ(mp->setVolume(vol), media::PLAYER_OK);
	}

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, setVolumeWithOutOfRangeValue)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	EXPECT_EQ(mp->setVolume(-1), media::PLAYER_ERROR);
	EXPECT_EQ(mp->setVolume(32), media::PLAYER_ERROR);

	mp->unprepare();
	mp->destroy();
}

TEST_F(SimpleMediaPlayerTest, getVolume)
{
	mp->create();
	mp->setDataSource(std::move(source));
	mp->prepare();

	for (int vol = 0; vol <= 31; vol++) {
		mp->setVolume(vol);
		EXPECT_EQ(mp->getVolume(), vol);
	}

	mp->unprepare();
	mp->destroy();
}
