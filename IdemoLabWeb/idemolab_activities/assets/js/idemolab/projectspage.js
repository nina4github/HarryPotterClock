function create_projects_page(timeline){
	
//	console.log(timeline);

		var people = get_people_from_results(timeline);
		var peopleLatestTweet = get_people_latest_tweet(people,timeline);
		//var activities = get_activities_from_results(timeline);

		var columns = 3;

		$.each(all_projects,function(index,project){

			if (index%columns==0){
				//console.log("index: "+index);						
				$('#boxes-container').append('<div class="row-fluid boxes-row-'+(index/columns)+'">');
			}

			$('#boxes-container .row-fluid.boxes-row-'+Math.floor(index/columns)).append(build_project_box(project));	
			console.log(project);

			//add people whose latest tweet is in this project
			var projectPeople = peopleLatestTweet.filter(function (item){
					 return item.tweet.entities.hashtags.filter(function (hashtag){
						return hashtag.text===project
					}).length > 0
				});


			$('.box.'+project).append(populate_project_box(project,projectPeople));	
		});
	}

	function build_project_box(project){
		var type = get_project_type(project);
		//console.log(type);
		var box ='<div class="span4 project box-'+type+' box '+project+' well"><h2>'+project+'</h2></div><!--/span-->';
		return box;
	}

	function populate_project_box(project,projectPeople){

		var person_content = '';	
		$.each(projectPeople, function(k,v){
			var userimage = "http://api.twitter.com/1/users/profile_image/"+v.name+".jpg";
			var userimage = "../assets/img/userimage.png";
			person_content += '<p class="span6"> <a href="#" class="thumbnail span4"> <img src="'+ userimage +'" alt=""></a><span class="span8">'+v.name+'</span></p>';
			//person_content += '<span class="span4">'+v.name+'</span>';
		});

		var box_content = '<div class="row-fluid project_details"> '
						+	person_content
						+'</div><!--/row-->';

		return box_content;
	}

	function get_project_tweets(project, timeline){
		var tweets = timeline.filter(function (result) { 
							return result.entities.hashtags.filter(function (hashtag){
								return hashtag.text===project
							}).length > 0;
						});

		return tweets;
	}



	function get_project_type(project){
		if ($.inArray(project,danger_projects)>-1) {return "danger";}
		else if ($.inArray(project,warning_projects)>-1) {return "warning";}
		else if ($.inArray(project,success_projects)>-1) {return "success";}
		else return "info";
	}